#include "motors.hpp"

//// プライベート変数・クラス・関数 ////

static DSR1202 *_dsr = nullptr;

static float _deg_position[4] = {0}; // モータの物理的な配置角度
static int _move_sign[4] = {1};      // 移動ベクトル計算後のパワーに対する符号
static int _pd_sign[4] = {1};        // PD制御の回転トルクに対する符号

static PD *_pd = nullptr;
static PD *_last_pd = nullptr;

//// パブリッククラス ////

// モーター制御の初期化
bool motorsInit(HardwareSerial *serial, uint32_t baudrate)
{
    if (_dsr != nullptr)
    {
        delete _dsr;
    }

    _dsr = new DSR1202(serial, baudrate);
    _dsr->begin();

    if (_pd == nullptr)
    {
        _pd = new PD(0.0f, 0.0f);
    }

    return true;
}

// モータの起動トグルスイッチのピン設定
void motorsSetTogglePin(uint8_t pin, uint8_t pinmode)
{
    if (_dsr != nullptr)
    {
        _dsr->setTogglePin(pin, pinmode);
    }
}

// 各モーターの物理的な配置角度を設定
void motorsSetDegPosition(float deg_1ch, float deg_2ch, float deg_3ch, float deg_4ch)
{
    _deg_position[0] = deg_1ch > 180 ? deg_1ch - 360 : deg_1ch;
    _deg_position[1] = deg_2ch > 180 ? deg_2ch - 360 : deg_2ch;
    _deg_position[2] = deg_3ch > 180 ? deg_3ch - 360 : deg_3ch;
    _deg_position[3] = deg_4ch > 180 ? deg_4ch - 360 : deg_4ch;
}

// 各モーターの移動方向の符号を設定（1または-1）
void motorsSetMoveSign(int sign_1ch, int sign_2ch, int sign_3ch, int sign_4ch)
{
    _move_sign[0] = sign_1ch / abs(sign_1ch);
    _move_sign[1] = sign_2ch / abs(sign_2ch);
    _move_sign[2] = sign_3ch / abs(sign_3ch);
    _move_sign[3] = sign_4ch / abs(sign_4ch);
}

// 各モーターのPD制御（回転トルク）の符号を設定（1または-1）
void motorsSetPdSign(int sign_1ch, int sign_2ch, int sign_3ch, int sign_4ch)
{
    _pd_sign[0] = sign_1ch / abs(sign_1ch);
    _pd_sign[1] = sign_2ch / abs(sign_2ch);
    _pd_sign[2] = sign_3ch / abs(sign_3ch);
    _pd_sign[3] = sign_4ch / abs(sign_4ch);
}

// PDの計算を実行
void motorsPdProcess(PD *pd, float deg, float target)
{
    _pd = pd;

    if (_pd != _last_pd)
    {
        _pd->reset(deg);
    }
    _pd->process(deg, target, true);

    _last_pd = _pd; // 記録
}

void motorsDirectMove(int value_1ch, int value_2ch, int value_3ch, int value_4ch)
{
    _dsr->move(value_1ch, value_2ch, value_3ch, value_4ch);
}

#define PD_MAX 80.0f
#define PD_MOVING_MAX 20.0f

void motorsMove(float deg, float power)
{
    // 角度による計算
    deg = -normalizeDeg(deg + 180);
    float powers[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    for (int i = 0; i < 4; i++)
        powers[i] = -sinf(radians(deg + _deg_position[i])) * float(power * _move_sign[i]);

    // 最大出力を探す
    float strongest_abs_power = 0.0f;
    for (int i = 0; i < 4; i++)
    {
        if (fabsf(powers[i]) > strongest_abs_power)
        {
            strongest_abs_power = fabsf(powers[i]);
        }
    }
    // 最大出力にするためにスケーリング
    float scale_for_max = power / strongest_abs_power;
    if (strongest_abs_power != 0.0f)
    {
        for (int i = 0; i < 4; i++)
        {
            powers[i] = (powers[i] * scale_for_max);
        }
    }

    // PID制御の値を加算したパワーを一時的にtemp_powersに格納
    float pd_value = constrain(_pd->output(), -PD_MOVING_MAX, PD_MOVING_MAX);
    float temp_powers[4];
    for (int i = 0; i < 4; i++)
    {
        temp_powers[i] = powers[i] + pd_value * float(_pd_sign[i]);
    }

    // 最大出力を再度探索
    strongest_abs_power = 0.0f;
    for (int i = 0; i < 4; i++)
    {
        if (fabsf(temp_powers[i]) > strongest_abs_power)
        {
            strongest_abs_power = fabsf(temp_powers[i]);
        }
    }

    // 100を超える用であれば移動成分（powers）のみを削る形で補正
    if (strongest_abs_power > 100.0f)
    {
        // PD値を優先し、入り切らない分を移動成分から引くための係数を計算
        // 各モーターにおいて |powers[i] * move_scale + pd_component| <= 100 を満たす必要がある
        float move_scale = 1.0f;
        for (int i = 0; i < 4; i++)
        {
            float pd_component = pd_value * float(_pd_sign[i]);
            // 移動成分がある場合のみ計算
            if (fabsf(powers[i]) > 0.001f)
            {
                // (100 - |pd成分|) / |移動成分| で、そのモーターが許容できる最大のスケーリング比を出す
                float limit_scale = (100.0f - fabsf(pd_component)) / fabsf(powers[i]);
                if (limit_scale < move_scale)
                {
                    move_scale = limit_scale;
                }
            }
        }
        // move_scaleが負にならないよう制限（PD値だけで100を超えているケース対策）
        if (move_scale < 0.0f)
            move_scale = 0.0f;

        // 移動成分を一律でスケーリング
        for (int i = 0; i < 4; i++)
        {
            powers[i] *= move_scale;
        }
    }

    // 補正済みPD値をpowersに加算
    for (int i = 0; i < 4; i++)
    {
        powers[i] += pd_value * float(_pd_sign[i]);
        powers[i] = constrain(powers[i], -100.0f, 100.0f); // 安全策
    }

    // 制御
    _dsr->move((int)roundf(powers[0]), (int)roundf(powers[1]), (int)roundf(powers[2]), (int)roundf(powers[3]));
}

// PD制御のみで機体を回転させる
void motorsPdMove()
{
    float pd_output = _pd->output();
    pd_output = constrain(pd_output, -PD_MAX, PD_MAX);

    int output1 = (int)roundf(pd_output * float(_pd_sign[0]));
    int output2 = (int)roundf(pd_output * float(_pd_sign[1]));
    int output3 = (int)roundf(pd_output * float(_pd_sign[2]));
    int output4 = (int)roundf(pd_output * float(_pd_sign[3]));

    // 制御
    _dsr->move(output1, output2, output3, output4);
}

// 全てのモーターを停止させる
void motorsStop()
{
    if (_dsr != nullptr)
    {
        _dsr->stop();
    }
}

void motorsVectorMove(Vector *vec)
{
    motorsMove(vec->deg(), vec->length());
}
