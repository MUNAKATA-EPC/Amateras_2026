#include "attacker.hpp"

/*攻撃用のプログラムを実行する*/

// Timer attacker_line_timer; // ラインセンサー用のタイマー

void play_attacker(bool use_cam, int motor_power)
{
    /*PD制御*/
    if (use_cam)
    {
        // カメラを使う
        if (goal_select_toggle.is_turn_on())
            PD_use_yellow_goal(); // 黄色ゴールをPD制御に使う
        else
            PD_use_blue_goal(); // 青ゴールをPD制御に使う
    }
    else
    {
        // ジャイロを使う
        PD_use_gyro(); // PD制御にジャイロを使う
    }

    /*ロボット制御*/
    if (is_LINE_exist()) // ラインがあるならば
    {
        motors_move(get_LINE_deg() + 180, motor_power); // ラインから逃れる動きをする
    }
    else
    {
        if (is_IR_exist()) // IRボールがあるならば
        {
            if ((get_IR_deg() < 4) || (get_IR_deg() > 356))
            {
                motors_move(0, motor_power);

                /*キッカー制御*/
                // kicker_kick(get_catchsensor()); // キャッチセンサーが反応したら蹴る
                kicker_kick(get_IR_distance() < 250); // 前付近で近くのボールがあったら蹴る
            }
            else if ((get_IR_deg() < 80) || (get_IR_deg() > 280))
            {
                motors_move(get_IR_hirei_deg(2.3), motor_power);
            }
            else
            {
                motors_move(get_IR_mawarikomi_deg(), motor_power);
            }
        }
        else
        {
            motors_only_PD(95); // PD制御のみで動く
        }
    }
}