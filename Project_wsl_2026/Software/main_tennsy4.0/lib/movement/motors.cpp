#include "motors.hpp"

/*計算して指定した出力・方向に動くようにモーターを制御する*/
/*モーターはすべて正で、まっすぐ進むようにする*/

int motor_deg[4];             // モータの設置角度を格納用
int motor_move_power_main[4]; // モータのスケーリングされた出力格納用(移動のための出力)
int motor_pd_power_main[4];   // モータのスケーリングされた出力格納用(PD制御のための出力)
int motor_power_main[4];      // 最終的な出力格納用

int motor_pid_sign[4] = {1, -1, -1, 1}; // モータを回転させるための符号を格納

void motors_init(int deg_1ch, int deg_2ch, int deg_3ch, int deg_4ch)
{
    motor_deg[0] = deg_1ch;
    motor_deg[1] = deg_2ch;
    motor_deg[2] = deg_3ch;
    motor_deg[3] = deg_4ch;
}

void motors_move(int deg, int abs_power)
{
    abs_power = abs(abs_power); // 一応絶対値にする（めんどくさいから絶対値にしている）

    compute_motor_power(deg, 100); // 適当に100の出力での各モータの出力を計算する

    double sum_x_power = 0; // x方向の出力の合計
    double sum_y_power = 0; // y方向の出力の合計
    for (int i = 0; i < 4; i++)
    {
        sum_x_power += get_motor_x_power(i); // iチャンネルのモーターのx出力を加算
        sum_y_power += get_motor_y_power(i); // iチャンネルのモーターのy出力を加算
    }

    double power_xy = sqrt(pow(sum_x_power, 2) + pow(sum_y_power, 2)); // xy合力の出力を計算

    if (power_xy > 1e-6) // 0よりも大きいとき
    {
        double scale_value = abs_power / power_xy; // 比率を求める
        for (int i = 0; i < 4; i++)
        {
            motor_move_power_main[i] = int(get_motor_power(i) * scale_value);                      // スケーリングしてmotor_power_main[]に格納
            motor_move_power_main[i] = constrain(motor_move_power_main[i], -abs_power, abs_power); // 一応納める
        }
    }
    else // 0で割ってはいけないので
    {
        for (int i = 0; i < 4; i++)
        {
            motor_move_power_main[i] = 0; // 出力をゼロに
        }
    }

    for (int i = 0; i < 4; i++)
    {
        motor_power_main[i] = motor_move_power_main[i] + get_PD_power() * motor_pid_sign[i]; // 移動-PD制御で最終的な出力を出す
        motor_power_main[i] = constrain(motor_power_main[i], -abs_power, abs_power);         // 一応丸める
    }

    DSR1202_move(motor_power_main[0], motor_power_main[1], motor_power_main[2], motor_power_main[3]); // モータを動かす
}

/*ここのファイル内だけで使う関数*/

double motor_move_power_sub[4]; // 計算したモータ出力git
double motor_x_power[4];        // モータ出力をx方向に分解した出力
double motor_y_power[4];        // モータ出力をy方向に分解した出力

// degはロボット全体として進みたい方向
// powerはその方向へ進むときの合力の大きさ
void compute_motor_power(int deg, int power)
{
    for (int i = 0; i < 4; i++)
    {
        // sin(進みたい角度 - モータ角度) × powerがモータごとの必要出力となる
        motor_move_power_sub[i] = sin(radians(deg - motor_deg[i])) * power;

        // タイヤの接線方向（モータ角度+90度方向）に力が働くので、その方向に分解
        int power_deg = motor_deg[i] + 90;

        // モータの出力をx,y方向に分解した値を計算
        motor_x_power[i] = cos(radians(power_deg)) * motor_move_power_sub[i];
        motor_y_power[i] = sin(radians(power_deg)) * motor_move_power_sub[i];
    }
}

// index番目のモータの合力の大きさを返す
double get_motor_power(int index)
{
    return motor_move_power_sub[index];
}

// index番目のモータの出力をx方向に分解した値を返す
double get_motor_x_power(int index)
{
    return motor_x_power[index];
}

// index番目のモータの出力をy方向に分解した値を返す
double get_motor_y_power(int index)
{
    return motor_y_power[index];
}
