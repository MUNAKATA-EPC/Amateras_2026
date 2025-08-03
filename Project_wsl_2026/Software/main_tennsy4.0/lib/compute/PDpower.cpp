#include "PDpower.hpp"

#define GYRO_P_GAIN 0.8
#define GYRO_D_GAIN 0.6

int pd_power;            // PD制御の出力格納用
double p_power, d_power; // P・Dそれぞれの出力格納

int now_gyro_value = 0;    // 今の角度格納用(-180~180)
int old_gyro_value = 0;    // 昔の角度格納用(-180~180)
int gap_of_gyro_value = 0; // 今と昔の差格納用(-180~180)

Timer gyro_timer;            // ジャイロ用のタイマーを定義
bool gyro_first_call = true; // 関数が最初に呼び出されたかを読む

void PD_use_gyro()
{
    now_gyro_value = 180 - BNO055_getDeg();              // BNO055の角度を-180~180にして保存
    gap_of_gyro_value = now_gyro_value - old_gyro_value; // 差を計算
    old_gyro_value = now_gyro_value;                     // BNO055の昔の角度を-180~180にして保存

    /*P制御について*/
    p_power = now_gyro_value * GYRO_P_GAIN; // 角度のずれ様によって比例制御

    /*D制御について*/
    if (gyro_first_call)
    {
        gyro_timer.reset();      // タイマーをリセット
        gyro_first_call = false; // 次からは初回ではない

        d_power = 0.0; // 最初の呼び出しなので値は0
    }
    else
    {
        double gyro_timer_sec = gyro_timer.get_time() / 1000.0; // msからsに変える

        if (gyro_timer_sec > 1e-6)
        {
            d_power = GYRO_D_GAIN * gap_of_gyro_value / gyro_timer_sec; // deg/sec * GYRO_D_GAINでD制御の出力を求める
        }
        else
        {
            d_power = 0.0; // 除算はだめなので0にする
        }

        gyro_timer.reset(); // タイマーをリセット
    }

    /*足し合わせる*/
    pd_power = (int)(p_power + d_power);       // それぞれの出力を足し合わせる
    pd_power = constrain(pd_power, -100, 100); // 一応-100~100に収める
}

void PD_use_cam()
{
}

int get_PD_power()
{
    return pd_power; // 計算したPD出力を返す
}