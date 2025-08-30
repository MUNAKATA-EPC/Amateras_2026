#include "radicon.hpp"
#include "attacker.hpp"
#include "defender.hpp"

/*ラジコン用のプログラムを実行する*/

void play_radicon(int motor_power)
{
    bool is_IR_exist_front = is_IR_exist() && ((get_IR_deg() <= 11) || (get_IR_deg() >= 349)); // 前にボールがあるかどうか格納

    /*PD制御*/
    if (is_Ps3_stick_right_move()) // 右ステックが倒されたならば
        PD_use_gyro(get_Ps3_stick_right_deg());
    else
        PD_use_gyro(0); // PD制御にジャイロを使う

    /*キッカー制御*/
    if (get_Ps3_button_r1() || get_Ps3_button_r2())
        kicker_kick(true); // ボタンが反応したら蹴る
    else if (is_IR_exist_front && get_IR_distance() <= 16 && get_selected_ui_setting(KICK_AUTO_INDEX))
        kicker_kick(true); // 前付近で近くのボールがありsettingで有効ならば蹴る
    else
        kicker_kick(false); // 蹴らない

    /*ロボット制御*/
    int motor_power_variable;
    if (get_Ps3_button_circle())
        motor_power_variable = 99; // 〇ボタンが押されたら99で動かす
    else
        motor_power_variable = map(get_Ps3_stick_left_distance(), 0, 128, 0, motor_power); // 左ステックの倒されようによってスピードを変える

    if (is_LINE_exist() && get_selected_ui_setting(LINE_AUTO_INDEX)) // ラインがあるかつsettingで有効ならば
    {
        motors_move(get_LINE_memory_deg() + 180, motor_power); // ラインから逃れる動きをする
    }
    else if (is_Ps3_stick_left_move()) // 左スティックが倒されたならば
    {
        motors_move((get_Ps3_stick_left_deg() + get_BNO055_deg() + 360) % 360, motor_power_variable); // 左ステックの方向へ行く
    }
    else if (is_IR_exist() && (get_Ps3_button_l1() || get_Ps3_button_l2()) && get_selected_ui_setting(ATTACK_AUTO_INDEX)) // IRボールがあってl3ボタンが押されsettingで有効ならば
    {
        if (is_IR_exist()) // IRボールがあるならば
        {
            if (is_IR_exist_front) // 前にボールがあるならば
            {
                motors_move(0, motor_power); // 前進する
            }
            else if (((get_IR_deg() <= 40) || (get_IR_deg() >= 320)) && get_IR_distance() < 25) // 前付近にボールがあるならば
            {
                motors_move(get_IR_hirei_deg(2.47), motor_power);
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
                int diff_deg = get_IR_hirei_value(0.226, 0); // 差を格納

                if (get_IR_deg() < 180) // 左にボールがある場合
                    mawarikomi_deg = (get_IR_deg() + diff_deg + 360) % 360;
                else
                    mawarikomi_deg = (get_IR_deg() - diff_deg + 360) % 360;

                // Serial.println(diff_deg);

                motors_move(mawarikomi_deg, motor_power);
            }
        }
        else
        {
            motors_only_PD(motor_power); // PD制御のみで動く
        }
    }
    else
    {
        motors_only_PD(motor_power); // PD制御のみで動く
    }
}