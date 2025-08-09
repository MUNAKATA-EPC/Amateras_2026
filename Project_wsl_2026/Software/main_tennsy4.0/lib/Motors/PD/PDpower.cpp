#include "PDpower.hpp"

/*PD制御の出力値を計算する*/

#define GYRO_P_GAIN 0.6
#define GYRO_D_GAIN 0.02

#define GOAL_P_GAIN 0.35
#define GOAL_D_GAIN 0.02

int pd_power;            // PD制御の出力格納用
double p_power, d_power; // P・Dそれぞれの出力格納

int now_gyro_value = 0;    // 今の角度格納用(-180~180)
int old_gyro_value = 0;    // 昔の角度格納用(-180~180)
int gap_of_gyro_value = 0; // 今と昔の差格納用(-180~180)

Timer gyro_timer;            // ジャイロ用のタイマーを定義
bool gyro_first_call = true; // 関数が最初に呼び出されたかを読む

void PD_use_gyro()
{
    now_gyro_value = (get_BNO055_deg() < 180) ? get_BNO055_deg() : get_BNO055_deg() - 360; // BNO055の角度を-180~180にして保存
    gap_of_gyro_value = now_gyro_value - old_gyro_value;                                   // 差を計算
    old_gyro_value = now_gyro_value;                                                       // BNO055の昔の角度を-180~180にして保存

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

    /*合計*/
    pd_power = (int)(p_power + d_power);       // それぞれの出力を足し合わせる
    pd_power = constrain(pd_power, -100, 100); // 一応-100~100に収める
}

double yellow_goal_p_power, yellow_goal_d_power; // P成分・D成分

int now_yellow_goal_value = 0;    // 今の黄色ゴール角度
int old_yellow_goal_value = 0;    // 前回の黄色ゴール角度
int gap_of_yellow_goal_value = 0; // 今と前回の差

Timer yellow_goal_timer;            // タイマー
bool yellow_goal_first_call = true; // 最初の呼び出しかどうか

void PD_use_yellow_goal()
{
    if (is_yellow_goal_exist())
    {
        now_yellow_goal_value = (get_yellow_goal_deg() < 180) ? get_yellow_goal_deg() : get_yellow_goal_deg() - 360; // 現在の角度
        gap_of_yellow_goal_value = now_yellow_goal_value - old_yellow_goal_value;                                    // 差を計算
        old_yellow_goal_value = now_yellow_goal_value;

        /*P制御*/
        yellow_goal_p_power = now_yellow_goal_value * GOAL_P_GAIN;

        /*D制御*/
        if (yellow_goal_first_call)
        {
            yellow_goal_timer.reset();
            yellow_goal_first_call = false;
            yellow_goal_d_power = 0.0;
        }
        else
        {
            double yellow_timer_sec = yellow_goal_timer.get_time() / 1000.0;

            if (yellow_timer_sec > 1e-6)
            {
                yellow_goal_d_power = GOAL_D_GAIN * gap_of_yellow_goal_value / yellow_timer_sec;
            }
            else
            {
                yellow_goal_d_power = 0.0;
            }

            yellow_goal_timer.reset();
        }

        /*合計*/
        pd_power = (int)(yellow_goal_p_power + yellow_goal_d_power);
        pd_power = constrain(pd_power, -100, 100);
    }
    else
    {
        yellow_goal_first_call = true; // 次に見えたときに初回処理させる

        // ゴールが見えないのでジャイロで処理する
        PD_use_gyro();
    }
}

double blue_goal_p_power, blue_goal_d_power; // P成分・D成分

int now_blue_goal_value = 0;    // 今の青色ゴール角度
int old_blue_goal_value = 0;    // 前回の青色ゴール角度
int gap_of_blue_goal_value = 0; // 今と前回の差

Timer blue_goal_timer;            // タイマー
bool blue_goal_first_call = true; // 最初の呼び出しかどうか

void PD_use_blue_goal()
{
    if (is_blue_goal_exist())
    {
        now_blue_goal_value = (get_blue_goal_deg() < 180) ? get_blue_goal_deg() : get_blue_goal_deg() - 360; // 現在の角度
        gap_of_blue_goal_value = now_blue_goal_value - old_blue_goal_value;                                  // 差を計算
        old_blue_goal_value = now_blue_goal_value;

        /*P制御*/
        blue_goal_p_power = now_blue_goal_value * GOAL_P_GAIN;

        /*D制御*/
        if (blue_goal_first_call)
        {
            blue_goal_timer.reset();
            blue_goal_first_call = false;
            blue_goal_d_power = 0.0;
        }
        else
        {
            double blue_timer_sec = blue_goal_timer.get_time() / 1000.0;

            if (blue_timer_sec > 1e-6)
            {
                blue_goal_d_power = GOAL_D_GAIN * gap_of_blue_goal_value / blue_timer_sec;
            }
            else
            {
                blue_goal_d_power = 0.0;
            }

            blue_goal_timer.reset();
        }

        /*合計*/
        pd_power = (int)(blue_goal_p_power + blue_goal_d_power);
        pd_power = constrain(pd_power, -100, 100);
    }
    else
    {
        blue_goal_first_call = true; // 次に見えたときに初回処理させる

        // ゴールが見えないのでジャイロで処理する
        PD_use_gyro();
    }
}

int get_PD_power()
{
    return -pd_power; // 計算したPD出力を返す
}