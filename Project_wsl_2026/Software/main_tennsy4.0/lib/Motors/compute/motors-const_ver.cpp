#include "motors-const_ver.hpp"

/*すべてのモータの合力が常に指定した値になるようにモーターを制御する*/

/*モーターはすべて正で、まっすぐ進むようにする*/
int motor_pid_sign[4] = {1, -1, -1, 1}; // モータを回転させるための符号を格納

int motor_deg[4];             // モータの設置角度を格納用
int motor_move_power_main[4]; // モータのスケーリングされた出力格納用(移動のための出力)
int motor_pd_power_main[4];   // モータのスケーリングされた出力格納用(PD制御のための出力)
int motor_power_main[4];      // 最終的な出力格納用

void motors_init(int deg_1ch, int deg_2ch, int deg_3ch, int deg_4ch)
{
    motor_deg[0] = deg_1ch;
    motor_deg[1] = deg_2ch;
    motor_deg[2] = deg_3ch;
    motor_deg[3] = deg_4ch;
}

void motors_move(int deg, int abs_power)
{
    abs_power = abs(abs_power); // 一応絶対値にする

    compute_motor_power(deg, 100); // 適当に100の出力での各モータの出力を計算

    float sum_x_power = 0; // x方向の出力の合計
    float sum_y_power = 0; // y方向の出力の合計
    for (int i = 0; i < 4; i++)
    {
        sum_x_power += get_motor_x_power(i);
        sum_y_power += get_motor_y_power(i);
    }

    float power_xy = sqrt(pow(sum_x_power, 2) + pow(sum_y_power, 2));

    if (power_xy > 1e-6)
    {
        float scale_value = float(abs_power) / power_xy;
        for (int i = 0; i < 4; i++)
        {
            motor_move_power_main[i] = int(get_motor_power(i) * scale_value);
            motor_move_power_main[i] = constrain(motor_move_power_main[i], -abs_power, abs_power);
        }
    }
    else
    {
        for (int i = 0; i < 4; i++)
        {
            motor_move_power_main[i] = 0;
        }
    }

    for (int i = 0; i < 4; i++)
    {
        motor_power_main[i] = motor_move_power_main[i] + get_PD_power() * motor_pid_sign[i];
        motor_power_main[i] = constrain(motor_power_main[i], -abs_power, abs_power);
    }

    DSR1202_move(motor_power_main[0], motor_power_main[1], motor_power_main[2], motor_power_main[3]);
}

/*ここのファイル内だけで使う関数*/

float motor_move_power_sub[4]; // 計算したモータ出力
float motor_x_power[4];        // モータ出力をx方向に分解
float motor_y_power[4];        // モータ出力をy方向に分解

void compute_motor_power(int deg, int power)
{
    for (int i = 0; i < 4; i++)
    {
        motor_move_power_sub[i] = sin(radians(deg - motor_deg[i])) * power;

        int power_deg = motor_deg[i] + 90;

        motor_x_power[i] = cos(radians(power_deg)) * motor_move_power_sub[i];
        motor_y_power[i] = sin(radians(power_deg)) * motor_move_power_sub[i];
    }
}

float get_motor_power(int index)
{
    return motor_move_power_sub[index];
}

float get_motor_x_power(int index)
{
    return motor_x_power[index];
}

float get_motor_y_power(int index)
{
    return motor_y_power[index];
}
