#include <Arduino.h>
#include "USE_m5pico.h"
#include "USE_DSR1202.h"
#include "USE_Kicker.h"
#include "USE_BNO.h"
#include "PID.h"

#define CONTROLMAXPOWER 20
#define GYROMAXPOWER 11

const int m5pico_TalkStart_ms = 4500;

void setup()
{
    // PCとのシリアル
    Serial.begin(115200);
    // BNOとの通信を開始させる
    pinMode(22, INPUT_PULLDOWN);
    BNObegin(22);
    // PIDの初期設定
    PIDbegin(0.078, 0.027, 15.5, GYROMAXPOWER, 3);
    // M5-picoの設定
    M5PICObegin(9600);  // D6をRX,D7をTXとしている。通信速度は9600でM5-picoと通信開始
    Stick_Adjust(8, 8); // AnalogStickの-1とする範囲の調整。X・Yどちらも±5は-1としている。
    // キッカーの設定
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
    Kickerbegin(2, 3);
    // シリアル1でDSRと通信する
    DSR1202begin(1);
}

void loop()
{
    M5PICOget(); // M5-picoからの値を更新

    // R1、R2、L1、L2のいずれかが押されるとキックする
    CheckKick(SW_right1 + SW_left1 + SW_right2 + SW_left2);

    // DSRを動かす
    double ControlLevel = 1.0;
    if (SW_circle)
        ControlLevel = 1.8;
    double GyroMotorVal = PID(BNOread());
    double ControlPower[4] =
        {-sin(radians(LStick_Angle - 60)) * CONTROLMAXPOWER * (LStick_Angle != -1) * ControlLevel,
         sin(radians(LStick_Angle - 300)) * CONTROLMAXPOWER * (LStick_Angle != -1) * ControlLevel,
         -sin(radians(LStick_Angle - 135)) * CONTROLMAXPOWER * (LStick_Angle != -1) * ControlLevel,
         sin(radians(LStick_Angle - 225)) * CONTROLMAXPOWER * (LStick_Angle != -1) * ControlLevel};

    DSR1202move(int(ControlPower[0] + GyroMotorVal),
                int(ControlPower[1] - GyroMotorVal),
                int(ControlPower[2] + GyroMotorVal),
                int(ControlPower[3]) - GyroMotorVal);

    Serial.println(LStick_Dist);

    delay(100);
}