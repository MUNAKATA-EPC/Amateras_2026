#include "motors.hpp"

static DSR1202 *_dsr = nullptr;

static int _deg_position[4] = {0}; // モータの物理的な配置角度
static int _move_sign[4] = {1};    // 移動ベクトル計算後のパワーに対する符号
static int _pd_sign[4] = {1};      // PD制御の回転トルクに対する符号

static PD *_pd = nullptr;

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
    _pd_sign[2] = sign_3ch / abs(sign_2ch);
    _pd_sign[3] = sign_4ch / abs(sign_4ch);
}

// PDの計算を実行
void motorsPdProcess(PD *pd, int deg, int target)
{
    _pd = pd;
    _pd->process(deg, target, true);
}

#define PID_MAX 80.0f
#define PID_MOVING_MAX 20.0f

void motorsMove(int deg, int power)
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
        if (fabs(powers[i]) > strongest_abs_power)
        {
            strongest_abs_power = fabs(powers[i]);
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

    // PID制御の値を加算
    for (int i = 0; i < 4; i++)
    {
        float pd_value = _pd->output() * float(_pd_sign[i]);

        if (powers[i] > -50.0f && powers[i] < 50.0f)
        {
            powers[i] += pd_value;
        }
        else
        {
            if (pd_value < 0.0f)
            {
                if (pd_value < -PID_MOVING_MAX)
                {
                    pd_value = -PID_MOVING_MAX;
                }
            }
            else
            {

                if (pd_value > PID_MOVING_MAX)
                {
                    pd_value = PID_MOVING_MAX;
                }
            }

            powers[i] += pd_value;
        }
    }

    // 最大出力を再度探索
    strongest_abs_power = 0.0f;
    for (int i = 0; i < 4; i++)
    {
        if (fabs(powers[i]) > strongest_abs_power)
        {
            strongest_abs_power = fabs(powers[i]);
        }
    }
    // PDの値によって値が100より大きくなったとき補正する
    if (strongest_abs_power > 100.0f)
    {
        float beyond_power = strongest_abs_power - 100.0f;

        for (int i = 0; i < 4; i++)
        {
            if (powers[i] > 0.0f)
                powers[i] = powers[i] - beyond_power;
            else
                powers[i] = powers[i] + beyond_power;
        }
    }
    else
    {
        for (int i = 0; i < 4; i++)
        {
            powers[i] = constrain(powers[i], -100.0f, 100.0f);
        }
    }

    // 制御
    _dsr->move((int)roundf(powers[0]), (int)roundf(powers[1]), (int)roundf(powers[2]), (int)roundf(powers[3]));
}

// PD制御のみで機体を回転させる
void motorsPdMove()
{
    float pd_output = _pd->output();

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
    motorsMove(vec->deg(), (int)roundf(vec->length()));
}
