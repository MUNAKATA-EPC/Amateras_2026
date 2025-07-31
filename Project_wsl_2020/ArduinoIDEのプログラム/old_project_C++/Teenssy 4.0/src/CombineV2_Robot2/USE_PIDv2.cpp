#include <Arduino.h>
#include <PID_v1.h>
#include "USE_PIDv2.h"

int MotorMax = 0;                         // モーターの最大値
int Frontdeg = 0;                         // 前とする角度
double PIDGyroData;                       // センサ値
double GyromotorVal;                      // モーター出力
double GyroGoalVal = 0.0;                 // 目標値
double Kp = 0.215, Ki = 0.01, Kd = 0.033; // PIDゲイン

// オブジェクトとして定義
PID GyroPID(&PIDGyroData, &GyromotorVal, &GyroGoalVal, Kp, Ki, Kd, DIRECT);

void PIDv2begin(double a, double b, double c, int d, int e)
{
    // ゲインを再設定
    Kp = a;
    Ki = b;
    Kd = c;
    GyroPID.SetTunings(Kp, Ki, Kd);

    // モーターの最大出力
    MotorMax = d;

    // 角度のデッドゾーン
    Frontdeg = e;

    // 初期化：一度MANUALにしてからAUTOMATICに戻すことで内部状態をリセット
    GyroPID.SetMode(MANUAL);
    GyroPID.SetOutputLimits(-d, d);
    GyroPID.SetMode(AUTOMATIC);
}

double PIDv2(double deg)
{
    PIDGyroData = deg;

    // ±180°の範囲に変換
    if (PIDGyroData > 180.0)
        PIDGyroData -= 360.0;

    // デッドゾーン処理（微小角度では制御しない）
    if (abs(PIDGyroData) <= Frontdeg)
        PIDGyroData = 0.0;

    // PID計算
    GyroPID.Compute();

    return GyromotorVal;
}

