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
            if (is_LINE_half_out())
            {
                motors_move(get_LINE_deg() + 180, motor_power);
            }
            else
            {
                int LINE_follow_deg1 = (get_LINE_deg() + 90 + 360) % 360;                                          // ライン接線1
                int LINE_follow_deg2 = (get_LINE_deg() - 90 + 360) % 360;                                          // ライン接線2
                if ((get_IR_deg() - LINE_follow_deg1 + 360) % 360 < (get_IR_deg() - LINE_follow_deg2 + 360) % 360) // IR_degと比べてどっちの接線が近いか
                {
                    // deg1が近い
                    motors_move(LINE_follow_deg1, motor_power);
                }
                else
                {
                    // deg2が近い
                    motors_move(LINE_follow_deg2, motor_power);
                }
            }
        }
        else
        {
            motors_move(get_LINE_deg() + 180, motor_power);
        }
    } /*
     else if (old_defene_movement && defence_timer.get_time() < 80 && !(get_IR_deg() <= 30 || get_IR_deg() >= 330))
     {
         motors_move(old_defence_deg, motor_power);
     }*/
    else
    {
        if (goal_select_toggle.is_turn_on())
        {
            if (is_blue_goal_exist())
                motors_move(get_blue_goal_deg(), motor_power); // 黄色攻めなのでDefenderは青ゴールに向かう
            else
            {
                motors_move(180, motor_power); // ゴール見えないので後ろに下がる
            }
        }
        else
        {
            if (is_yellow_goal_exist())
                motors_move(get_yellow_goal_deg(), motor_power); // 青色攻めなのでDefenderは黄ゴールに向かう
            else
            {
                motors_move(180, motor_power); // ゴール見えないので後ろに下がる
            }
        }
    }
}