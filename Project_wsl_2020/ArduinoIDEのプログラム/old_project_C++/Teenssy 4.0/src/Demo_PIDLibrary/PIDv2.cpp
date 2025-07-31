#include <Arduino.h>
#include <PID_v1.h>

int MotorMax = 0;                         // モーターの最大値
int Frontdeg = 0;                         // 前とする角度
double PIDGyroData;                       // センサ値など
double GyromotorVal;                      // モーター出力など
double GyroGoalVal = 0.0;                 // 目標の値
double Kp = 0.215, Ki = 0.01, Kd = 0.033; // PIDゲイン

// PID制御のオブジェクトを作成（&で変数を渡す）
PID GyroPID(&PIDGyroData, &GyromotorVal, &GyroGoalVal, Kp, Ki, Kd, DIRECT);

void PIDv2begin(double a, double b, double c, int d, int e)
{
    // それぞれのゲインに代入
    Kp = a;
    Ki = b;
    Kd = c;
    // モーターの最大値を代入
    MotorMax = d;
    // 前とする角度を代入
    Frontdeg = e;

    // PIDを開始する
    GyroPID.SetMode(AUTOMATIC);     // PID制御を「自動モード」に設定（開始）
    GyroPID.SetOutputLimits(-d, d); // 出力値の範囲
}

double PIDv2(double deg)
{
    // 角度を計算
    PIDGyroData = deg;
    if (PIDGyroData > 180.0)
        PIDGyroData -= 360.0;
    if (abs(PIDGyroData) <= Frontdeg)
        PIDGyroData = 0.0;

    GyroPID.Compute(); // PID計算を実行（Output が自動で更新される）

    return (GyromotorVal);
}