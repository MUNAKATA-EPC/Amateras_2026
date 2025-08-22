#include "PDpower.hpp"

/*PD制御の出力値を計算する*/

#define GYRO_P_GAIN 0.8
#define GYRO_D_GAIN 0.06

#define GYRO_TARGET_P_GAIN 0.6
#define GYRO_TARGET_D_GAIN 0.02

#define GOAL_P_GAIN 0.35
#define GOAL_D_GAIN 0.02

int pd_power = 0; // PD制御の出力格納用
double p_power = 0.0, d_power = 0.0; // P・Dそれぞれの出力格納

int now_gyro_value = 0;    // 今の角度格納用(-180~180)
int old_gyro_value = 0;    // 昔の角度格納用(-180~180)
int gap_of_gyro_value = 0; // 今と昔の差格納用(-180~180)

Timer gyro_timer;             // ジャイロ用のタイマーを定義
bool gyro_first_call = true;  // 関数が最初に呼び出されたかを読む

void PD_use_gyro(int target_deg)
{
    int deg = get_BNO055_deg();
    deg = (deg + target_deg + 360) % 360;
    now_gyro_value = (deg < 180) ? deg : deg - 360;

    gap_of_gyro_value = now_gyro_value - old_gyro_value;
    if (gap_of_gyro_value > 180) gap_of_gyro_value -= 360;
    if (gap_of_gyro_value < -180) gap_of_gyro_value += 360;

    old_gyro_value = now_gyro_value;

    // P制御
    p_power = now_gyro_value * GYRO_P_GAIN;

    // D制御
    if (gyro_first_call)
    {
        gyro_timer.reset();
        gyro_first_call = false;
        d_power = 0.0;
    }
    else
    {
        double gyro_timer_sec = gyro_timer.get_time() / 1000.0;
        d_power = (gyro_timer_sec > 1e-6) ? GYRO_D_GAIN * gap_of_gyro_value / gyro_timer_sec : 0.0;
        gyro_timer.reset();
    }

    pd_power = (int)constrain(p_power + d_power, -100, 100);
}

// 黄色ゴールPD制御
double yellow_goal_p_power = 0.0, yellow_goal_d_power = 0.0;
int now_yellow_goal_value = 0, old_yellow_goal_value = 0, gap_of_yellow_goal_value = 0;
Timer yellow_goal_timer;
bool yellow_goal_first_call = true;

void PD_use_yellow_goal()
{
    if (is_yellow_goal_exist())
    {
        now_yellow_goal_value = (get_yellow_goal_deg() < 180) ? get_yellow_goal_deg() : get_yellow_goal_deg() - 360;
        gap_of_yellow_goal_value = now_yellow_goal_value - old_yellow_goal_value;
        if (gap_of_yellow_goal_value > 180) gap_of_yellow_goal_value -= 360;
        if (gap_of_yellow_goal_value < -180) gap_of_yellow_goal_value += 360;
        old_yellow_goal_value = now_yellow_goal_value;

        yellow_goal_p_power = now_yellow_goal_value * GOAL_P_GAIN;

        if (yellow_goal_first_call)
        {
            yellow_goal_timer.reset();
            yellow_goal_first_call = false;
            yellow_goal_d_power = 0.0;
        }
        else
        {
            double yellow_timer_sec = yellow_goal_timer.get_time() / 1000.0;
            yellow_goal_d_power = (yellow_timer_sec > 1e-6) ? GOAL_D_GAIN * gap_of_yellow_goal_value / yellow_timer_sec : 0.0;
            yellow_goal_timer.reset();
        }

        pd_power = (int)constrain(yellow_goal_p_power + yellow_goal_d_power, -100, 100);
    }
    else
    {
        yellow_goal_first_call = true;
        PD_use_gyro(0);
    }
}

// 青色ゴールPD制御
double blue_goal_p_power = 0.0, blue_goal_d_power = 0.0;
int now_blue_goal_value = 0, old_blue_goal_value = 0, gap_of_blue_goal_value = 0;
Timer blue_goal_timer;
bool blue_goal_first_call = true;

void PD_use_blue_goal()
{
    if (is_blue_goal_exist())
    {
        now_blue_goal_value = (get_blue_goal_deg() < 180) ? get_blue_goal_deg() : get_blue_goal_deg() - 360;
        gap_of_blue_goal_value = now_blue_goal_value - old_blue_goal_value;
        if (gap_of_blue_goal_value > 180) gap_of_blue_goal_value -= 360;
        if (gap_of_blue_goal_value < -180) gap_of_blue_goal_value += 360;
        old_blue_goal_value = now_blue_goal_value;

        blue_goal_p_power = now_blue_goal_value * GOAL_P_GAIN;

        if (blue_goal_first_call)
        {
            blue_goal_timer.reset();
            blue_goal_first_call = false;
            blue_goal_d_power = 0.0;
        }
        else
        {
            double blue_timer_sec = blue_goal_timer.get_time() / 1000.0;
            blue_goal_d_power = (blue_timer_sec > 1e-6) ? GOAL_D_GAIN * gap_of_blue_goal_value / blue_timer_sec : 0.0;
            blue_goal_timer.reset();
        }

        pd_power = (int)constrain(blue_goal_p_power + blue_goal_d_power, -100, 100);
    }
    else
    {
        blue_goal_first_call = true;
        PD_use_gyro(0);
    }
}

int get_PD_power()
{
    return -pd_power;
}
