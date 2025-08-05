#include "motors-boost_ver.hpp"

/*計算して指定した出力・方向に全力で動くようにモーターを制御する*/

/*モーターはすべて正で、右回転するようにする*/
int motor_move_sign[4] = {1, -1, -1, 1}; // モータを移動させるための符号を格納

int motor_deg[4];        // モータの設置角度を格納用
int motor_move_power[4]; // モータの出力格納用(移動のための出力)->これをスケーリングし、最大にする
int motor_pd_power[4];   // モータの出力格納用(PD制御のための出力)
int motor_power_main[4]; // 最終的な出力格納用

void motors_init(int deg_1ch, int deg_2ch, int deg_3ch, int deg_4ch)
{
    motor_deg[0] = deg_1ch + 90;
    motor_deg[1] = deg_2ch + 90;
    motor_deg[2] = deg_3ch + 90;
    motor_deg[3] = deg_4ch + 90;
}

void motors_move(int deg, int abs_power)
{
    abs_power = abs(abs_power); // 一応絶対値にする（めんどくさいから絶対値にしている）

    compute_motor_power(deg, 100); // 適当に100の出力での各モータの出力を計算する->後でスケーリングするため適当でよい

    double max_abs_power = 0; // 最大の出力を見つける
    for (int i = 0; i < 4; i++)
    {
        if (abs(motor_move_power[i]) > max_abs_power)
            max_abs_power = abs(motor_move_power[i]); // 大きかったら更新する
    }
    double scale_value_for_max = (max_abs_power == 0) ? 0 : abs_power / max_abs_power; // モータの出力を最大(abs_power)にできるような係数を計算

    for (int i = 0; i < 4; i++) // 最終的な計算
    {
        motor_power_main[i] = motor_move_power[i] * scale_value_for_max * motor_move_sign[i] + get_PD_power(); // 移動-PD制御で最終的な出力を出す
        motor_power_main[i] = constrain(motor_power_main[i], -abs_power, abs_power);                           // 一応丸める
    }

    DSR1202_move(motor_power_main[0], motor_power_main[1], motor_power_main[2], motor_power_main[3]); // モータを動かす
}

void motors_only_PD(int max_pd_power)
{
    for (int i = 0; i < 4; i++) // 最終的な計算
    {
        motor_power_main[i] = get_PD_power();                                              // 移動-PD制御で最終的な出力を出す
        motor_power_main[i] = constrain(motor_power_main[i], -max_pd_power, max_pd_power); // 一応丸める
    }

    DSR1202_move(motor_power_main[0], motor_power_main[1], motor_power_main[2], motor_power_main[3]); // モータを動かす
}

void motors_break()
{
    DSR1202_break(); // モータを停止させる
}

/*ここのファイル内だけで使う関数*/

void compute_motor_power(int deg, int power)
{
    deg = 360 - deg;
    for (int i = 0; i < 4; i++)
    {
        motor_move_power[i] = sin(radians(deg - motor_deg[i])) * power;
    }
}

double get_motor_power(int index)
{
    return motor_move_power[index];
}