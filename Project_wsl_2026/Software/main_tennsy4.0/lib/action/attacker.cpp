#include "attacker.hpp"

/*攻撃用のプログラムを実行する*/

void play_attacker(bool use_cam, int motor_power)
{
    /*キッカー制御*/
    // kicker_kick(get_catchsensor()); // キャッチセンサーが反応したら蹴る
    kicker_kick((is_IR_exist()) && ((get_IR_deg() < 20 || get_IR_deg() > 340) && (get_IR_distance() < 150))); // 前付近で近くのボールがあったら蹴る

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
            if (get_IR_deg() < 10 || get_IR_deg() > 350) // 前にIRボールがあるとき
            {
                motors_move(0, motor_power); // 前進する
            }
            else if (get_IR_deg() < 20) // 左にボールがあるとき
            {
                motors_move(90, motor_power); // 前方100方向にボールが行くように制御する
            }
            else if (get_IR_deg() > 340) // 右にボールがあるとき
            {
                motors_move(270, motor_power); // 前方100方向にボールが行くように制御する
            }
            else if (get_IR_deg() < 30 || get_IR_deg() > 330) // 前付近にIRボールがあるとき
            {
                motors_move(get_IR_follow_deg(280), motor_power); // 前方100方向にボールが行くように制御する
            }
            else
            {
                if (get_IR_distance() < 170) // 近くにIRボールがあるとき
                {
                    motors_move(get_IR_mawarikomi_deg(), motor_power); // 回り込むように制御
                }
                else
                {
                    motors_move(get_IR_follow_deg(0), motor_power); // ボールに近寄るように制御
                }
            }
        }
        else
        {
            motors_only_PD(motor_power); // PD制御のみで動く
        }
    }
}