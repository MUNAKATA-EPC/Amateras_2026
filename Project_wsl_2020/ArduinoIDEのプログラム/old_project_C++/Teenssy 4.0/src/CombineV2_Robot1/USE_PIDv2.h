#ifndef USE_PIDV2_H

#define USE_PIDV2_H

// PIDゲイン
extern double Kp, Ki, Kd;

// モーターの最大出力の変数
extern const int CONTROLMAXPOWER;
extern const int GYROMAXPOWER;

// PID制御のゲインとモーターの最大値と前とする角度を入力するための関数
void PIDv2begin(double a, double b, double c, int d, int e);

// PID制御の値を返す関数
double PIDv2(double deg);

#endif