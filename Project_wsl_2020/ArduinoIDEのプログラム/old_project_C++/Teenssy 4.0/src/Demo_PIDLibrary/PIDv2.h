#ifndef PIDV2_H

#define PIDV2_H

//PID制御のゲインとモーターの最大値と前とする角度を入力するための関数
void PIDv2begin(double a, double b, double c, int d, int e);

//PID制御の値を返す関数
double PIDv2(double deg);

#endif