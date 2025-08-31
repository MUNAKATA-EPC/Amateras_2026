#ifndef USE_PIDV2_H

#define USE_PIDV2_H

// PIDゲイン
extern double Kp, Ki, Kd;

// モーターの最大出力の変数
extern const int CONTROLMAXPOWER;
extern const int GYROMAXPOWER;

// PID制御のゲインと前とする角度を入力するための関数
void PIDv2begin();

// PID制御の値を返す関数
double PIDv2(double deg, double goaldeg, double maxValue);

#endif