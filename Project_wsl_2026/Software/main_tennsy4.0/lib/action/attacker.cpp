#include "attacker.hpp"

/*攻撃用のプログラムを実行する*/

// Timer attacker_line_timer; // ラインセンサー用のタイマー

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
        PD_use_gyro(); // PD制御にジャイロを使う
    }

    /*キッカー制御*/
    // kicker_kick(get_catchsensor() <= 300); // キャッチセンサーが反応したら蹴る

    /*ロボット制御*/
    if (is_LINE_exist()) // ラインがあるならば
    {
        if (is_LINE_half_out())
        {
            motors_move(get_LINE_deg() + 180, motor_power / 3); // ラインから逃れる動きをする
        }
        else
        {
            motors_move(get_LINE_deg() + 180, motor_power); // ラインから逃れる動きをする
        }
    }
    else
    {
        if (is_IR_exist()) // IRボールがあるならば
        {
            if ((get_IR_deg() < 3) || (get_IR_deg() > 357))
            {
                kicker_kick(get_IR_distance() < 300); // 前付近で近くのボールがあったら蹴る

                motors_move(0, motor_power); // 前進する
            }
            else if (get_IR_distance() < 300) // 近く
            {
                if ((get_IR_deg() < 80) || (get_IR_deg() > 280))
                {
                    motors_move(get_IR_hirei_deg(2.6), motor_power);
                }
                else
                {
                    motors_move(get_IR_mawarikomi_deg(), motor_power);
                }
            }
            else
            {
                motors_move(get_IR_follow_deg(0), motor_power);
            }
        }
        else
        {
            motors_only_PD(motor_power); // PD制御のみで動く
        }
    }
}