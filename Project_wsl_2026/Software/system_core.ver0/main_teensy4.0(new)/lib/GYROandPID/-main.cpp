#include <Arduino.h>
#include "MySettings.h"
#include "BNO055.h"
#include <PID_v1.h>

/*BNO055*/

Adafruit_BNO055 bno = Adafruit_BNO055(BNO055_ID, BNO055_ADRESS, &BNO055_WIRE);

int BNO055Resetdeg = 0;
int BNO055Normaldeg = 0;
int BNO055Finaldeg = 0;
int BNO055deg = 0;

void BNO055begin()
{
    while (!bno.begin(OPERATION_MODE_IMUPLUS))
        ; // BNO055と通信が確立するまで待つ
    bno.setExtCrystalUse(true);
    pinMode(BNO055_RESETSWPIN, BNO055_RESETSWPINMODE);
}

void BNO055reset()
{
    imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER); // BNO055からデータを取得

    BNO055Normaldeg = euler.x();      // X軸のデータを取得
    BNO055Resetdeg = BNO055Normaldeg; // ResetDegを更新
}

void BNO055get()
{
    imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER); // BNO055からデータを取得

    BNO055Normaldeg = euler.x();                                          // X軸のデータを取得
    BNO055Finaldeg = (int)(BNO055Normaldeg - BNO055Resetdeg + 360) % 360; // 角度を計算

    if (BNO055_RESETSWPINMODE == INPUT_PULLDOWN)
    {
        if (digitalRead(BNO055_RESETSWPIN) == HIGH)
        {
            BNO055Resetdeg = BNO055Normaldeg; // ResetDegを更新
        }
    }
    else
    {
        if (digitalRead(BNO055_RESETSWPIN) == LOW)
        {
            BNO055Resetdeg = BNO055Normaldeg; // ResetDegを更新
        }
    }

    BNO055deg = BNO055Finaldeg;
}

/*PID*/

int MotorMax = 0;                 // モーターの最大値
double Gyrodeg;                   // センサ値
double PIDmotorVal;               // モーター出力
double GoalGyrodeg = 0.0;         // 目標角度
double Kp = KP, Ki = KI, Kd = KD; // PIDゲイン

// オブジェクトとして定義
PID GyroPID(&Gyrodeg, &PIDmotorVal, &GoalGyrodeg, Kp, Ki, Kd, DIRECT);

void PIDv2begin()
{
    // 初期化：一度MANUALにしてからAUTOMATICに戻すことで内部状態をリセット
    GyroPID.SetMode(MANUAL);
    GyroPID.SetOutputLimits(-100, 100); // ±1023の間でGyromotorValに代入する
    GyroPID.SetMode(AUTOMATIC);
}

double PIDv2(double deg, double goaldeg, double maxValue)
{
    double error = deg - goaldeg;

    if (error > 180.0)
        error -= 360.0;
    else if (error < -180.0)
        error += 360.0;

    Gyrodeg = error;   // PID入力に誤差を渡す
    GoalGyrodeg = 0.0; // 目標は0度の偏差

    // デッドゾーン
    if (abs(error) <= FRONTDEG)
        Gyrodeg = 0.0;

    GyroPID.Compute();

    double returnData = map(PIDmotorVal, -100, 100, -maxValue, maxValue);
    return returnData;
}
