#include "Arduino.h"
#include "PID.h"

// それぞれのゲイン
float Kp, Ki, Kd;

// モーターの最大値
int MotorMax = 0;

// 前とする角度
int Frontdeg = 0;

// I制御関連の変数
float AndAnd_time;
long double I_LastTime = 0.0;
int Lastdeg = 0;

// D制御関係の変数
long double D_LastTime = 0.0;

void PIDbegin(float a, float b, float c, int d, int e)
{
    // それぞれのゲインに代入
    Kp = a;
    Ki = b;
    Kd = c;
    // モーターの最大値を代入
    MotorMax = d;
    // 前とする角度を代入
    Frontdeg = e;
}

float PID(int PIDdeg)
{
    // degを-180 ~ 180に変換
    if (PIDdeg > 180)
        PIDdeg -= 360;

    // P制御の計算
    float P_MotorVal = 0.0;
    P_MotorVal = PIDdeg * Kp;
    if (PIDdeg > 90)
        P_MotorVal = MotorMax;
    if (PIDdeg < -90)
        P_MotorVal = -MotorMax; // 角度が90~270度の時はジャイロは最大出力で調整

    // I制御の計算
    float I_MotorVal = 0.0;
    if (PIDdeg <= Lastdeg + 1 && PIDdeg >= Lastdeg - 1 && PIDdeg != 0)
    {
        AndAnd_time = millis() - I_LastTime;
        I_MotorVal = AndAnd_time * Ki * PIDdeg / abs(PIDdeg);
    }
    else
    {
        I_LastTime = millis();
        I_MotorVal *= 0.5;
    }

    // D制御の計算
    float D_MotorVal = 0.0;
    D_MotorVal = Kd * (Lastdeg - PIDdeg) / (millis() - D_LastTime);
    D_LastTime = millis();

    // 範囲内に抑え、rerunする
    float PID_val = P_MotorVal + I_MotorVal + D_MotorVal;
    if (abs(PID_val) > MotorMax)
    {
        if (PID_val > 0)
        {
            PID_val = MotorMax;
        }
        else
        {
            PID_val = -MotorMax;
        }
    }
    if (abs(PIDdeg) <= Frontdeg)
        PID_val = 0; // PIDdegがFrontdeg以内だったら出力は0にする

    Lastdeg = PIDdeg; // 昔の角度の記録

    return (PID_val);
}