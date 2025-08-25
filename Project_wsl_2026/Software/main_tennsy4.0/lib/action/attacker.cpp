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
    // kicker_kick(get_catchsensor() <= 300); // キャッチセンサーが反応したら蹴る

    /*ロボット制御*/
    if (is_LINE_exist()) // ラインがあるならば
    {
        if (is_LINE_half_out() || !is_IR_exist()) // IRボールがないもしくはハーフアウトしたなら
        {
            motors_move(get_LINE_memory_deg() + 180, motor_power); // ラインから逃れる動きをする
        }
        else // ハーフアウト前でIRボールがあるなら
        {
            /*
            if (compute_deg_diff(get_LINE_deg(), get_IR_deg()) < 60) // ラインの角度とIRの角度同じくらいの方向だった場合
            {
                // ゴールの接線方向もしくはラインの接線方向を計算（2通り）
                int defence_sessen_deg1 = (get_LINE_deg() + 90 + 360) % 360;
                int defence_sessen_deg2 = (get_LINE_deg() - 90 + 360) % 360;

                int diff_of_sessen1 = compute_deg_diff(defence_sessen_deg1, get_IR_deg()); // 差を計算
                int diff_of_sessen2 = compute_deg_diff(defence_sessen_deg2, get_IR_deg()); // 差を計算

                if (diff_of_sessen1 < diff_of_sessen2) // sessen_deg1のほうがボールの方向との差が小さいなら
                {
                    motors_move(defence_sessen_deg1, motor_power / 1.6);
                }
                else // sessen_deg2のほうがボールの方向との差が小さいなら
                {
                    motors_move(defence_sessen_deg2, motor_power / 1.6);
                }
            }
            else // ラインの角度とは反対の位置にIRボールがある場合
            {
                motors_move(get_IR_deg(), motor_power / 1);
            }
            */
            motors_move(get_LINE_memory_deg() + 180, motor_power);
        }
    }
    else
    {
        if (is_IR_exist()) // IRボールがあるならば
        {
            if ((get_IR_deg() <= 3) || (get_IR_deg() >= 357)) // 前にブールがあるならば
            {
                kicker_kick(get_IR_distance() < 300); // 前付近で近くのボールがあったら蹴る

                motors_move(0, motor_power); // 前進する
            }
            else if ((get_IR_deg() <= 80) || (get_IR_deg() >= 280)) // 前付近にボールがあるならば
            {
                if (get_IR_distance() < 300)
                {
                    motors_move(get_IR_hirei_deg(2), motor_power);
                }
                else
                {
                    motors_move(get_IR_sessen_deg(70, -170), motor_power);
                }
            }
            else
            {
                if (get_IR_distance() < 320)
                {
                    motors_move(get_IR_sessen_deg(100, -170), motor_power);
                }
                else
                {
                    motors_move(get_IR_deg(), motor_power);
                }
            }
        }
        else
        {
            motors_only_PD(motor_power); // PD制御のみで動く
        }
    }
}