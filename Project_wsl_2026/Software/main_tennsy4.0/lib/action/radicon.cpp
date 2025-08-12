#include "radicon.hpp"
#include "attacker.hpp"
#include "defender.hpp"

/*ラジコン用のプログラムを実行する*/

void play_radicon(int motor_power)
{
    /*PD制御*/
    if (is_Ps3_stick_right_move())                             // 右ステックが倒されたならば
        PD_use_gyro_set_target_ver(get_Ps3_stick_right_deg()); // PD制御にジャイロを使う
    else
        PD_use_gyro(); // PD制御にジャイロを使う

    /*キッカー制御*/
    kicker_kick(get_Ps3_button_l1() || get_Ps3_button_l2() || get_Ps3_button_r1() || get_Ps3_button_r2()); // ボタンが反応したら蹴る

    /*ロボット制御*/
    int motor_power_variable = map(get_Ps3_stick_left_distance(), 0, 128, 0, motor_power); // 左ステックの倒されようによってスピードを変える
    if (!get_Ps3_button_circle())
        motor_power_variable *= 0.75; // ◎ボタンが押されていないなら減速する

    if (is_LINE_exist()) // ラインがあるならば
    {
        motors_move(get_LINE_deg() + 180, motor_power_variable); // ラインから逃れる動きをする
    }
    else if (is_IR_exist() && get_Ps3_button_l3()) // IRボールがあってl3ボタンが押されたら
    {
        play_attacker(false, false, motor_power_variable); // 攻撃モードに切り替える
    }
    else if (is_Ps3_stick_left_move()) // 左スティックが倒されたならば
    {
        motors_move(get_Ps3_stick_left_deg(), motor_power_variable); // 左ステックの方向へ行く
    }
    else
    {
        motors_only_PD(motor_power_variable); // PD制御のみで動く
    }
}