#include "radicon.hpp"
#include "attacker.hpp"
#include "defender.hpp"

/*守備用のプログラムを実行する*/

int16_t old_line_memory_deg; // 昔のラインセンサーの角度格納用
Timer ir_follow_timer;
bool line_old_flag = false;

void play_defender(bool use_yellow_cam, bool use_blue_cam, int16_t motor_power)
{
    PD_use_gyro(0);

    if (is_LINE_exist())
    {
        line_old_flag = true;
        ir_follow_timer.reset();

        if (is_yellow_goal_exist())
            motors_move(get_yellow_goal_deg() + 180, motor_power / 2);
        else
            motors_move(get_LINE_memory_deg() + 180, motor_power / 2);

        old_line_memory_deg = get_LINE_memory_deg();
    }
    else
    {
        if ((line_old_flag || ir_follow_timer.get_time() < 250) && ir_follow_timer.get_time() != -1)
        {
            if (line_old_flag)
            {
                ir_follow_timer.reset(); // タイマースタート
                line_old_flag = false;
            }

            if (is_IR_exist())
            {
                // ゴールの接線方向もしくはラインの接線方向を計算（2通り）
                int defence_sessen_deg1 = (is_yellow_goal_exist()) ? (get_yellow_goal_deg() + 90 + 360) % 360 : (old_line_memory_deg + 90 + 360) % 360;
                int defence_sessen_deg2 = (is_yellow_goal_exist()) ? (get_yellow_goal_deg() - 90 + 360) % 360 : (old_line_memory_deg - 90 + 360) % 360;

                int diff_of_sessen1 = compute_deg_diff(defence_sessen_deg1, get_IR_deg()); // 差を計算
                int diff_of_sessen2 = compute_deg_diff(defence_sessen_deg2, get_IR_deg()); // 差を計算

                if (diff_of_sessen1 < diff_of_sessen2) // sessen_deg1のほうがボールの方向との差が小さいなら
                {
                    motors_move(defence_sessen_deg1, motor_power / 2);
                }
                else // sessen_deg2のほうがボールの方向との差が小さいなら
                {
                    motors_move(defence_sessen_deg2, motor_power / 2);
                }
            }
            else
            {
                motors_only_PD(motor_power);
            }
        }
        else
        {
            if (is_yellow_goal_exist())
                motors_move(get_yellow_goal_deg(), motor_power / 2);
            else
                motors_move(180, motor_power / 2);
        }
    }
    /*
    if (is_LINE_exist())
    {
        if (is_LINE_half_out())
        {
            motors_move(get_LINE_memory_deg() + 180, motor_power / 2);
        }
        else
        {
            if (is_IR_exist())
            {
                int line_sessen_deg1 = (get_LINE_deg() + 90 + 360) % 360;
                int line_sessen_deg2 = (get_LINE_deg() - 90 + 360) % 360;

                int diff_of_sessen1 = compute_deg_diff(line_sessen_deg1, get_IR_deg()); // 差を計算
                int diff_of_sessen2 = compute_deg_diff(line_sessen_deg2, get_IR_deg()); // 差を計算

                if (diff_of_sessen1 < diff_of_sessen2) // sessen_deg1のほうがボールの方向との差が小さいなら
                {
                    motors_move(line_sessen_deg1, motor_power);
                }
                else // sessen_deg2のほうがボールの方向との差が小さいなら
                {
                    motors_move(line_sessen_deg2, motor_power);
                }
            }
            else
            {
                motors_only_PD(motor_power);
            }
        }
    }
    else
    {
        if (is_yellow_goal_exist())
            motors_move(get_yellow_goal_deg(), motor_power / 2);
        else
            motors_move(180, motor_power / 2);
    }
    */
}