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
    bool is_IR_exist_front = is_IR_exist() && ((get_IR_deg() <= 9) || (get_IR_deg() >= 351)); // 前にボールがあるかどうか格納

    /*PD制御*/
    if (use_yellow_cam)
    {
        if (is_IR_exist_front && get_IR_value() > 185)
            PD_use_yellow_goal(); // 黄色ゴールをPD制御に使う
        else
            PD_use_gyro(0);
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
    kicker_kick(is_IR_exist_front && get_IR_value() > 185); // キャッチセンサーが反応したら蹴る

    /*ロボット制御*/
    if (is_LINE_exist()) // ラインがあるならば
    {
        motors_move(get_LINE_memory_deg() + 180, motor_power); // ラインから逃れる動きをする
    }
    else
    {
        if (is_IR_exist()) // IRボールがあるならば
        {
            if (is_IR_exist_front) // 前にボールがあるならば
            {
                motors_move(0, motor_power); // 前進する
            }
            else if (((get_IR_deg() <= 40) || (get_IR_deg() >= 320)) && get_IR_distance() < 300) // 前付近にボールがあるならば
            {
                motors_move(get_IR_hirei_deg(2.15), motor_power);
            }
            else
            {
                /*方法1*/
                // motors_move(get_IR_sessen_deg(85, -150), motor_power);

                /*方法2*/
                /*
                if (get_IR_distance() < 300)
                    motors_move(get_IR_mawarikomi_deg(), motor_power);
                else
                    motors_move(get_IR_follow_deg(0), motor_power);
                */

                /*方法3*/
                int mawarikomi_deg;
                int diff_deg = get_IR_hirei_distance(0.32, 0); // 差を格納

                if (get_IR_deg() < 180) // 左にボールがある場合
                    mawarikomi_deg = (get_IR_deg() + diff_deg + 360) % 360;
                else
                    mawarikomi_deg = (get_IR_deg() - diff_deg + 360) % 360;

                Serial.println(diff_deg);

                motors_move(mawarikomi_deg, motor_power);
            }
        }
        else
        {
            motors_only_PD(motor_power); // PD制御のみで動く
        }
    }
}