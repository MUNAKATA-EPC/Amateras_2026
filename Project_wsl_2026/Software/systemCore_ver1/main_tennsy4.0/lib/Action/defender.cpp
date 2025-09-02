#include "radicon.hpp"
#include "attacker.hpp"
#include "defender.hpp"

/*守備用のプログラムを実行する*/

void play_defender(bool use_yellow_cam, bool use_blue_cam, int motor_power)
{
    PD_use_gyro(0);

    if (is_field_exist())
    {
    }
    else
    {
        play_attacker(use_yellow_cam, use_blue_cam, motor_power);
    }

    /*
    if (is_LINE_exist())
    {
        if (is_yellow_goal_exist() && (get_yellow_goal_deg() > 140 && get_yellow_goal_deg() < 220))
        {
            if (get_LINE_deg() > 140 && get_LINE_deg() < 220)
            {
                motors_only_PD(motor_power);
            }
            else
            {
                motors_move(get_yellow_goal_deg() + 180, motor_power);
            }
        }
        else if (is_field_exist())
        {
            // ラインの接線方向を計算（2通り）
            int defence_sessen_deg1 = (get_LINE_memory_deg() + 90 + 360) % 360;
            int defence_sessen_deg2 = (get_LINE_memory_deg() - 90 + 360) % 360;

            int diff_of_sessen1 = compute_deg_diff(defence_sessen_deg1, get_field_deg()); // 差を計算
            int diff_of_sessen2 = compute_deg_diff(defence_sessen_deg2, get_field_deg()); // 差を計算

            if (diff_of_sessen1 < diff_of_sessen2) // sessen_deg1のほうがボールの方向との差が小さいなら
            {
                motors_move(defence_sessen_deg1, motor_power);
            }
            else // sessen_deg2のほうがボールの方向との差が小さいなら
            {
                motors_move(defence_sessen_deg2, motor_power);
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
        {
            motors_move(get_yellow_goal_deg(), motor_power);
        }
        else
        {
            motors_move(180, motor_power);
        }
    }
    */
}