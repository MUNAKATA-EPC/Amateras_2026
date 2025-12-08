// motors.cpp

#include "motors.hpp"

// DSR1202モータードライバクラスのポインタ
static DSR1202 *_dsr = nullptr;

// メンバ変数（staticで定義することでヘッダーを省略）
static int _deg_position[4] = {0};     // モータの物理的な配置角度（例: 315, 45, 225, 135など）
static int _move_sign[4] = {1};        // 移動ベクトル計算後のパワーに対する符号
static int _pd_sign[4] = {1};          // PD制御の回転トルクに対する符号
static float _move_output[4] = {0.0f}; // 移動成分の出力（スケーリング済み）
static int _output[4] = {0};           // 最終的なモーター出力（移動 + PD）

// PD制御クラスのポインタ
static PD *_pd = nullptr;

// トグルスイッチのピン番号
static uint8_t _toggle_pin = 0xFF;

// モーター制御の初期化
// serial 使用するシリアルポート
// baudrate 通信速度
// bool 初期化成功/失敗
bool motorsInit(HardwareSerial *serial, uint32_t baudrate)
{
    // 既存のインスタンスがあれば削除
    if (_dsr != nullptr)
    {
        delete _dsr;
    }
    // 新しいインスタンスを生成
    _dsr = new DSR1202(serial, baudrate);
    _dsr->begin();

    // PDクラスのインスタンスを確保（未設定時にNULLポインタ参照を防ぐため）
    if (_pd == nullptr)
    {
        _pd = new PD(0.0f, 0.0f);
    }

    return true;
}

// モータの起動トグルスイッチのピン設定
void motorsSetTogglePin(uint8_t pin, uint8_t pinmode)
{
    _toggle_pin = pin;

    pinMode(_toggle_pin, pinmode);
}

// 各モーターの物理的な配置角度を設定（度数法）
void motorsSetDegPosition(int deg_1ch, int deg_2ch, int deg_3ch, int deg_4ch)
{
    _deg_position[0] = deg_1ch > 180 ? deg_1ch - 360 : deg_1ch;
    _deg_position[1] = deg_2ch > 180 ? deg_2ch - 360 : deg_2ch;
    _deg_position[2] = deg_3ch > 180 ? deg_3ch - 360 : deg_3ch;
    _deg_position[3] = deg_4ch > 180 ? deg_4ch - 360 : deg_4ch;
}

// 各モーターの移動方向の符号を設定（1または-1）
void motorsSetMoveSign(int sign_1ch, int sign_2ch, int sign_3ch, int sign_4ch)
{
    // 常に1または-1に正規化
    _move_sign[0] = sign_1ch / abs(sign_1ch);
    _move_sign[1] = sign_2ch / abs(sign_2ch);
    _move_sign[2] = sign_3ch / abs(sign_3ch);
    _move_sign[3] = sign_4ch / abs(sign_4ch);
}

// 各モーターのPD制御（回転トルク）の符号を設定（1または-1）
void motorsSetPdSign(int sign_1ch, int sign_2ch, int sign_3ch, int sign_4ch)
{
    // 常に1または-1に正規化
    _pd_sign[0] = sign_1ch / abs(sign_1ch);
    _pd_sign[1] = sign_2ch / abs(sign_2ch);
    _pd_sign[2] = sign_3ch / abs(sign_2ch);
    _pd_sign[3] = sign_4ch / abs(sign_4ch);
}

// 制御系

// 全てのモーターを停止させる
void motorsStop()
{
    if (_dsr != nullptr)
    {
        _dsr->stop();
    }
}

// PD制御器の計算を実行
// pd 使用するPDオブジェクトのポインタ
// deg 現在の角度（BNOなど）
// target 目標角度
void motorsPdProcess(PD *pd, int deg, int target)
// ここでPDポインタを更新し、計算を実行
{
    _pd = pd;
    _pd->process(deg, target, true);
}

// 移動制御とPD制御を合成してモーターを駆動
// deg 移動したい方向（0: 正面）
// power 移動の強さ（最大値）
void motorsMove(int deg, int power)
{
    if (_toggle_pin != 0xFF)
    {
        // トグルスイッチがオフならモーターを停止して終了
        if (digitalRead(_toggle_pin) == LOW)
        {
            motorsStop();
            return;
        }
    }

    power = constrain(power, 0, 100); // powerを0~100に制限

    // 1. 移動成分の計算と最大出力の探索
    float max_move_output = 0.0f;
    for (int i = 0; i < 4; i++)
    {
        // モーターiから見た移動方向は (モーター位置 _deg_position[i]) - (移動方向 deg) で計算する
        Vector vec(_deg_position[i] - deg, power);

        // "i"chの出力(y成分だけが移動に使える)
        _move_output[i] = vec.y() * _move_sign[i];

        // 最大移動出力（絶対値）を更新
        max_move_output = (fabs(_move_output[i]) > max_move_output) ? fabs(_move_output[i]) : max_move_output;
    }

    // 2. 移動成分の出力スケーリング
    // 出力が power になるようにするためのスケールを計算
    float scale = (max_move_output != 0.0f) ? (float)power / max_move_output : 0.0f;
    for (int i = 0; i < 4; i++)
    {
        // 移動出力をスケーリング
        _move_output[i] *= scale;
    }

    // 4. PD成分を付け加えて最終出力を決定
    // PD制御の出力は -100~100 (int)
    float pd_output = (float)_pd->output();

    for (int i = 0; i < 4; i++)
    {
        // 最終出力 = 移動成分 + PD回転成分
        float final_raw_output = _move_output[i] + pd_output * _pd_sign[i];

        // 最終出力を -power から power の範囲に制限し、intに丸める
        // constrain の引数を double に明示的にキャスト
        _output[i] = (int)round(constrain(final_raw_output, (float)-power, (float)power));
    }

    // 5. 制御
    _dsr->move(_output[0], _output[1], _output[2], _output[3]);
}

void motorsVectorMove(Vector *vec)
{
    motorsMove(vec->deg(), (int)round(vec->length()));
}

// PD制御のみで機体を回転させる
void motorsPdMove()
{
    if (_toggle_pin != 0xFF)
    {
        // トグルスイッチがオフならモーターを停止して終了
        if (digitalRead(_toggle_pin) == LOW)
        {
            motorsStop();
            return;
        }
    }

    // 制御
    // PD制御の出力は -100~100 (int)
    int pd_output = _pd->output();

    // 全てのモーターに同じPDトルクを適用（その場の回転）
    // モーターに適用するPD出力を、PD符号に基づいて決定
    int output1 = pd_output * _pd_sign[0];
    int output2 = pd_output * _pd_sign[1];
    int output3 = pd_output * _pd_sign[2];
    int output4 = pd_output * _pd_sign[3];

    // 制御
    _dsr->move(output1, output2, output3, output4);
}