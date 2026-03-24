#include "bno.hpp"

//// プライベート変数・クラス・関数 ////

static Adafruit_BNO055 *_bno = nullptr;

static float _deg_normal = 0;
static float _deg_reset = 0;
static float _deg = 0;

static MovementAverageDouble _ave(1); // 加速度用の移動平均
static double _speed_x = 0.0;

//// パブリック関数 ////

bool bnoInit(TwoWire *wire, uint8_t address)
{
    if (_bno != nullptr)
    {
        delete _bno;
        _bno = nullptr;
    }
    _bno = new Adafruit_BNO055(BNO055_ID, address, wire);

    Timer timer;
    timer.reset();
    bool success = false;
    while (timer.msTime() < 1000UL)
    {
        success = _bno->begin(OPERATION_MODE_IMUPLUS);
        if (success)
        {
            break;
        }
        else
        {
            delay(50);
        }
    }

    if (success)
    {
        _bno->setExtCrystalUse(true);
    }
    return success;
}

void bnoUpdate(bool resetbtn, uint16_t uss_right_speed, uint16_t uss_left_speed)
{
    if (_bno == nullptr)
        return;

    // 角度
    imu::Vector<3> euler = _bno->getVector(Adafruit_BNO055::VECTOR_EULER);
    float current_raw_x = (float)euler.x();
    _deg_normal = current_raw_x;

    if (resetbtn)
    {
        _deg_reset = _deg_normal;
    }

    _deg = fmodf(_deg_normal - _deg_reset + 360, 360);
    if (_deg > 180)
        _deg -= 360;

    // 速度計算
    uint32_t now_time = micros();
    static uint32_t last_time = now_time;
    if (now_time - last_time >= 10000)
    {
        // 線形加速度の取得
        imu::Vector<3> a = _bno->getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
        double a_x = a.x();
        static double last_a_x = a_x;

        if (abs(a_x) < 0.05)
            a_x = 0.0;

        // 時間差を「秒」で計算
        double delta_t = 1.0 /*double(now_time - last_time) / 1000000.0*/; // msで計算

        // 積分（台形近似）
        // delta_speed_x = (平均加速度) * 時間(s)
        double delta_speed_x = ((a_x + last_a_x) / 2.0) * delta_t;

        // ハイパスフィルタ（ドリフト除去）
        // 速度 = (前回の速度 + 変化量) * 減衰係数
        const double drift_reduction = 0.8;
        _speed_x = (_speed_x + delta_speed_x) * drift_reduction;

        if (a_x == 0 && abs(_speed_x) < 0.01)
        {
            _speed_x = 0;
        }

        last_a_x = a_x;
        last_time = now_time;
    }

    // 超音波による速度リセット（ドリフト予防）
    static bool no_movement = false;
    static Timer no_movement_timer;
    bool uss_right_detected = uss_right_speed != 0xFFFF, uss_left_detected = uss_left_speed != 0xFFFF;

    if (uss_right_detected && uss_left_detected) // どちらも速度の変化が無かったらリセット
    {
        if (fabsf(uss_right_speed) <= 1.0f && fabsf(uss_left_speed) <= 1.0f)
        {
            if (no_movement == false)
            {
                no_movement_timer.reset();
            }

            if (no_movement_timer.msTime() > 1500UL) // 1500ms間動いていないと判断したら
            {
                _speed_x = 0.0; // リセット
            }

            no_movement = true; // 記録
        }
        else
        {
            no_movement = false; // 記録
        }
    }
    else
    {
        no_movement = false; // 記録
    }
}

float bnoDeg() { return _deg; }

double bnoSpeedX() { return _speed_x; }