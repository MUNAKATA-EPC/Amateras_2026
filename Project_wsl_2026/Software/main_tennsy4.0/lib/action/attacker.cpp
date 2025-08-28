#include "attacker.hpp"

/*攻撃用のプログラムを実行する*/

// 角度における"差"を計算する関数
int compute_deg_diff(int a, int b)
{
    int diff = (a - b + 360) % 360;
    if (diff > 180)
        diff = 360 - diff;
    return diff;
}

void play_attacker(bool use_yellow_cam, bool use_blue_cam, int motor_power)
{
    /*PD制御*/
    if (use_yellow_cam)
    {
        PD_use_yellow_goal(); // 黄色ゴールをPD制御に使う
    }
    else if (use_blue_cam)
    {
        PD_use_blue_goal(); // 黄色ゴールをPD制御に使う
    }
    else
    {
        // ジャイロを使う
        PD_use_gyro(0); // PD制御にジャイロを使う
    }

    /*キッカー制御*/
    kicker_kick(((get_IR_deg() <= 3) || (get_IR_deg() >= 357)) && get_catchsensor() <= 290); // キャッチセンサーが反応したら蹴る

    /*ロボット制御*/
    if (is_LINE_exist()) // ラインがあるならば
    {
        motors_move(get_LINE_memory_deg() + 180, motor_power); // ラインから逃れる動きをする
    }
    else
    {
        if (is_IR_exist()) // IRボールがあるならば
        {
            if ((get_IR_deg() <= 3) || (get_IR_deg() >= 357)) // 前にブールがあるならば
            {
                motors_move(0, motor_power); // 前進する
            }
            else if (((get_IR_deg() <= 80) || (get_IR_deg() >= 280)) && get_IR_distance() < 290) // 前付近にボールがあるならば
            {
                motors_move(get_IR_hirei_deg(2.7), motor_power);
            }
            else
            {
                motors_move(get_IR_sessen_deg(80, -150), motor_power);
            }
        }
        else
        {
            motors_only_PD(motor_power); // PD制御のみで動く
        }
    }
}