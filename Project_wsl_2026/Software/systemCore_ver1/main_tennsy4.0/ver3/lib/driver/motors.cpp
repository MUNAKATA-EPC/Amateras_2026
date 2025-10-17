// motors.cpp

#include "motors.hpp"

// DSR1202モータードライバクラスのポインタ
inline DSR1202 *_dsr = nullptr;

// メンバ変数（inlineで定義することでヘッダーを省略）
inline int _deg_position[4] = {0};     // モータの物理的な配置角度（例: 315, 45, 225, 135など）
inline int _move_sign[4] = {1};        // 移動ベクトル計算後のパワーに対する符号
inline int _pd_sign[4] = {1};          // PD制御の回転トルクに対する符号
inline double _move_output[4] = {0.0}; // 移動成分の出力（スケーリング済み）
inline int _output[4] = {0};           // 最終的なモーター出力（移動 + PD）

// PD制御クラスのポインタ
inline PD *_pd = nullptr;

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
        _pd = new PD(0.0, 0.0);
    }

    return true;
}

// 各モーターの物理的な配置角度を設定（度数法）
void motorsSetDegPosition(int deg_1ch, int deg_2ch, int deg_3ch, int deg_4ch)
{
    _deg_position[0] = deg_1ch;
    _deg_position[1] = deg_2ch;
    _deg_position[2] = deg_3ch;
    _deg_position[3] = deg_4ch;
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
    _pd->process(deg, target);
}

// 移動制御とPD制御を合成してモーターを駆動
// deg 移動したい方向（0: 正面）
// power 移動の強さ（最大値）
void motorsMove(int deg, int power)
{
    // 1. degの基準調整 (元のコードはコメントアウト維持)
    // deg = (deg + 180) % 360;

    // 2. 移動成分の計算と最大出力の探索
    double max_move_output = 0.0;
    for (int i = 0; i < 4; i++)
    {
        // **修正点**: モーターiから見た移動方向は (移動方向 deg) - (モーター位置 _deg_position[i]) で計算します。
        Vector vec(deg - _deg_position[i], power); // 正しい相対角度の計算

        // "i"chの出力(X成分だけが移動に使える)
        _move_output[i] = vec.x() * _move_sign[i];

        // 最大移動出力（絶対値）を更新
        max_move_output = (fabs(_move_output[i]) > max_move_output) ? fabs(_move_output[i]) : max_move_output;
    }

    // 3. 移動成分の出力スケーリング
    // 出力が power になるようにするためのスケールを計算
    double scale = (max_move_output != 0) ? (double)power / max_move_output : 0;
    for (int i = 0; i < 4; i++)
    {
        // 移動出力をスケーリング
        _move_output[i] *= scale;
    }

    // 4. PD成分を付け加えて最終出力を決定
    // PD制御の出力は -100~100 (int)
    // NOTE: _pd->output() は int を返すため、doubleにキャストし、元のコードの符号を維持。
    double pd_output = (double)-_pd->output();

    for (int i = 0; i < 4; i++)
    {
        // 最終出力 = 移動成分 + PD回転成分
        double final_raw_output = _move_output[i] + pd_output * _pd_sign[i];

        // 最終出力を -power から power の範囲に制限し、intに丸める
        // constrain の引数を double に明示的にキャスト
        _output[i] = (int)round(constrain(final_raw_output, (double)-power, (double)power));
    }

    // 5. 制御
    _dsr->move(_output[0], _output[1], _output[2], _output[3]);
}

// PD制御のみで機体を回転させる
void motorsPdMove()
{
    // 制御
    // PD制御の出力は -100~100 (int)
    // 元のコードの `-` 符号を維持。
    int pd_output = -_pd->output();

    // 全てのモーターに同じPDトルクを適用（その場の回転）
    // モーターに適用するPD出力を、PD符号に基づいて決定
    int output1 = pd_output * _pd_sign[0];
    int output2 = pd_output * _pd_sign[1];
    int output3 = pd_output * _pd_sign[2];
    int output4 = pd_output * _pd_sign[3];

    // 制御
    _dsr->move(output1, output2, output3, output4);
}

PD *pdGyro = new PD(0.6, 0.1); // ジャイロ用のPD調節値
PD *pdCam = new PD(0.6, 0.1);  // カメラ用のPD調節値