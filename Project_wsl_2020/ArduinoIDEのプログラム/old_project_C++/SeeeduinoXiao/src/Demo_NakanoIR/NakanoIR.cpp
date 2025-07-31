#include <Arduino.h>

const int SensorX[8] = {0, -141, -200, -141, 0, 141, 200, 141}; // ロボットについているセンサーのそれぞれのX座標
const int SensorY[8] = {200, 141, 0, -141, -200, -141, 0, 141}; // ロボットについているセンサーのそれぞれのY座標
double Weight[8];
double Weightx[8];
double Weighty[8];   // それぞれのセンサーの重さ
double Total_Weight; // それぞれのセンサーの重さのトータル
double TotalX;       // X座標付き全センサーの重さ
double TotalY;       // Y座標付き全センサーの重さ
double ball_X;
double ball_Y;
double Ball_angle;
int sensor_values[8];

void setup()
{
    Serial.begin(9600); // シリアル通信の速度を設定
    pinMode(0, OUTPUT);
    for (int i = 1; i <= 8; i++)
    {
        pinMode(i, INPUT);
    }
}

void loop()
{
    Total_Weight = 0.0;
    TotalX = 0.0;
    TotalY = 0.0;
    ball_X = 0.0;
    ball_Y = 0.0;

    for (int i = 0; i < 8; i++)
    {
        sensor_values[i] = analogRead(i + 1); // センサーの値を一つずつすべて読み取る。
    }

    for (int i = 0; i < 8; i++)
    {
        Weight[i] = 1023 - sensor_values[i]; // 各センサーの重さを算出
        Weight[i] /= 1000;
        Weightx[i] = Weight[i] * SensorX[i]; // X(Y)座標付き全センサーの重さの求め方：｛それぞれのセンサーの重さ×それぞれのセンサーのX(Y)座標｝
        Weighty[i] = Weight[i] * SensorY[i]; // ↑で１つのX座標付きセンサーの重さ求めることができ、1⃣と同じ原理でTotalを求める。
        TotalX += Weightx[i];
        TotalY += Weighty[i];
        Total_Weight += Weight[i]; // Total＿Weightがはじめ０なので重さの合計となる・・・1⃣
    }

    ball_X = TotalX / Total_Weight; //{X座標付き全センサーの重さ÷全センサーの重さ}でボールのX座標を求める
    ball_Y = TotalY / Total_Weight; //{Y　座標付き全センサーの重さ÷全センサーの重さ}でボールのX座標を求める
    Ball_angle = degrees(atan2(ball_X, ball_Y)) + 360;
    Ball_angle = (int(Ball_angle)) % 360;
    Serial.println("( " + String(ball_X) + " , " + String(ball_Y) + " ) -> " + String(Ball_angle));

    delay(100);
}