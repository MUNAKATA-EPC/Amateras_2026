#include "defender.hpp"

/*守備用のプログラムを実行する*/

void play_defender(bool use_cam_if_attacker, int motor_power)
{
    /*PD制御*/
    // ジャイロを使う
    PD_use_gyro(); // PD制御にジャイロを使う

    /*ロボット制御*/
    // Defenderでカメラは必ず使う
    Serial.print(is_LINE_half_out());
    if (is_LINE_exist()) // ラインがあるならば
    {
        if (is_IR_exist())
        {
            if (get_IR_deg() >= 0 && get_IR_deg() <= 180) // 左にボールがある
            {
                motors_move(get_LINE_deg() - 90, motor_power / 2);
            }
            else
            {
                motors_move(get_LINE_deg() + 90, motor_power / 2);
            }
        }
        else
        {
            motors_move(get_LINE_deg() + 180, motor_power / 2);
        }
    }
    else
    {
        if (goal_select_toggle.is_turn_on())
        {
            if (is_blue_goal_exist())
                motors_move(get_blue_goal_deg(), motor_power / 2); // 黄色攻めなのでDefenderは青ゴールに向かう
            else
            {
                motors_move(180, motor_power / 2); // ゴール見えないので後ろに下がる
            }
        }
        else
        {
            if (is_yellow_goal_exist())
                motors_move(get_yellow_goal_deg(), motor_power / 2); // 青色攻めなのでDefenderは黄ゴールに向かう
            else
            {
                motors_move(180, motor_power / 2); // ゴール見えないので後ろに下がる
            }
        }
    }
}