#include "PDpower.hpp"

/*PD制御の出力値を計算する*/

#define GYRO_P_GAIN 0.78
#define GYRO_D_GAIN 0.025

#define GYRO_TARGET_P_GAIN 0.6
#define GYRO_TARGET_D_GAIN 0.02

#define GOAL_P_GAIN 0.6
#define GOAL_D_GAIN 0.02

double pd_power_double = 0.0;       // PD制御の出力格納用(double版)
double p_power = 0.0, d_power = 0.0; // P・Dそれぞれの出力格納

double now_gyro_value = 0.0;    // 今の角度格納用(-180~180)
double old_gyro_value = 0.0;    // 昔の角度格納用(-180~180)
double gap_of_gyro_value = 0.0; // 今と昔の差格納用(-180~180)

Timer gyro_timer;             // ジャイロ用のタイマーを定義

void PD_use_gyro(int target_deg)
{
    double deg = get_BNO055_deg();
    deg = fmod(deg + target_deg + 360.0, 360.0);
    now_gyro_value = (deg < 180.0) ? deg : deg - 360.0;

    gap_of_gyro_value = now_gyro_value - old_gyro_value;
    if (gap_of_gyro_value > 180.0) gap_of_gyro_value -= 360.0;
    if (gap_of_gyro_value < -180.0) gap_of_gyro_value += 360.0;

    old_gyro_value = now_gyro_value;

    // P制御
    p_power = now_gyro_value * GYRO_P_GAIN;

    // D制御
    double gyro_timer_sec = 0.0;
    if (!gyro_timer.is_called_this_timer())
    {
        gyro_timer.reset();
        d_power = 0.0;
    }
    else
    {
        gyro_timer_sec = gyro_timer.get_time() / 1000.0;
        if (gyro_timer_sec > 1e-6)
            d_power = GYRO_D_GAIN * gap_of_gyro_value / gyro_timer_sec;
        else
            d_power = 0.0;
        gyro_timer.reset();
    }

    pd_power_double = constrain(p_power + d_power, -100.0, 100.0);
}

// 黄色ゴールPD制御
double yellow_goal_p_power = 0.0, yellow_goal_d_power = 0.0;
double now_yellow_goal_value = 0.0, old_yellow_goal_value = 0.0, gap_of_yellow_goal_value = 0.0;
Timer yellow_goal_timer;

void PD_use_yellow_goal()
{
    if (is_yellow_goal_exist())
    {
        double goal_deg = get_yellow_goal_deg();
        now_yellow_goal_value = (goal_deg < 180.0) ? goal_deg : goal_deg - 360.0;

        gap_of_yellow_goal_value = now_yellow_goal_value - old_yellow_goal_value;
        if (gap_of_yellow_goal_value > 180.0) gap_of_yellow_goal_value -= 360.0;
        if (gap_of_yellow_goal_value < -180.0) gap_of_yellow_goal_value += 360.0;

        old_yellow_goal_value = now_yellow_goal_value;

        yellow_goal_p_power = now_yellow_goal_value * GOAL_P_GAIN;

        if (!yellow_goal_timer.is_called_this_timer())
        {
            yellow_goal_timer.reset();
            yellow_goal_d_power = 0.0;
        }
        else
        {
            double yellow_timer_sec = yellow_goal_timer.get_time() / 1000.0;
            yellow_goal_d_power = (yellow_timer_sec > 1e-6) ? GOAL_D_GAIN * gap_of_yellow_goal_value / yellow_timer_sec : 0.0;
            yellow_goal_timer.reset();
        }

        pd_power_double = constrain(yellow_goal_p_power + yellow_goal_d_power, -100.0, 100.0);
    }
    else
    {
        PD_use_gyro(0);
    }
}

// 青色ゴールPD制御
double blue_goal_p_power = 0.0, blue_goal_d_power = 0.0;
double now_blue_goal_value = 0.0, old_blue_goal_value = 0.0, gap_of_blue_goal_value = 0.0;
Timer blue_goal_timer;

void PD_use_blue_goal()
{
    if (is_blue_goal_exist())
    {
        double goal_deg = get_blue_goal_deg();
        now_blue_goal_value = (goal_deg < 180.0) ? goal_deg : goal_deg - 360.0;

        gap_of_blue_goal_value = now_blue_goal_value - old_blue_goal_value;
        if (gap_of_blue_goal_value > 180.0) gap_of_blue_goal_value -= 360.0;
        if (gap_of_blue_goal_value < -180.0) gap_of_blue_goal_value += 360.0;

        old_blue_goal_value = now_blue_goal_value;

        blue_goal_p_power = now_blue_goal_value * GOAL_P_GAIN;

        if (!blue_goal_timer.is_called_this_timer())
        {
            blue_goal_timer.reset();
            blue_goal_d_power = 0.0;
        }
        else
        {
            double blue_timer_sec = blue_goal_timer.get_time() / 1000.0;
            blue_goal_d_power = (blue_timer_sec > 1e-6) ? GOAL_D_GAIN * gap_of_blue_goal_value / blue_timer_sec : 0.0;
            blue_goal_timer.reset();
        }

        pd_power_double = constrain(blue_goal_p_power + blue_goal_d_power, -100.0, 100.0);
    }
    else
    {
        PD_use_gyro(0);
    }
}

// 最終出力は int に丸めて返す
int get_PD_power()
{
    return -(int)(pd_power_double + 0.5); // 四捨五入して int に
}