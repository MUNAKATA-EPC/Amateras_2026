#ifndef PID_H

#define PID_H

//PID制御のゲインとモーターの最大値と前とする角度を入力するための関数
void PIDbegin(float a, float b, float c, int d, int e);

//傾きが90～270度の時以外はPID制御を行う
float PID(int deg);

//モータを動かすのに使う変数の設定
extern const int CONTROLMAXPOWER;
extern const int GYROMAXPOWER;

#endif