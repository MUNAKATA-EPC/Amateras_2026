#include <Arduino.h>
#include <SoftwareSerial.h>
#include "USE_m5pico.h"

SoftwareSerial mySerial(7, 8); // とりあえず設定

// M5-picoのデータを格納する変数
int left_X, left_Y, right_X, right_Y;
bool SW_left, SW_right, SW_up, SW_down;
bool SW_square, SW_circle, SW_triangle, SW_cross;
bool SW_left1, SW_left2, SW_right1, SW_right2;
bool SW_select, SW_start;
bool SWleftStick_left, SWleftStick_right, SWleftStick_up, SWleftStick_down;
bool SWrightStick_left, SWrightStick_right, SWrightStick_up, SWrightStick_down;
// AnalogStickの角度・距離を格納する変数
float LStick_Angle, LStick_Dist, RStick_Angle, RStick_Dist;
// AnalogStickの調節用の角度
int L_adjust, R_adjust;

// 昔のPs3のデータ
String Old_Ps3_Data = "0000,0000,0000,0000,00000000000000";

void M5PICObegin(int SerialSpeed)
{
    // ユーザーの設定した通信速度にする
    mySerial.begin(SerialSpeed);
}

void Stick_Adjust(int a, int b)
{
    // それぞれの調整用の変数に格納する
    L_adjust = a;
    R_adjust = b;
}

void M5PICOget()
{
    String Ps3_Data;

    // データを読み取る
    while (!mySerial.available())
    {
    }
    Ps3_Data = mySerial.readStringUntil('\n');

    if (Ps3_Data.length() != 35)
        Ps3_Data = Old_Ps3_Data; // 通信不良が起こった際に昔のデータを入れる
    if (millis() <= (unsigned long)m5pico_TalkStart_ms)
        Ps3_Data = "0000,0000,0000,0000,00000000000000"; // 電源を入れて3秒以内は通信不良を起こすかもなので"0000,0000,0000,0000,00000000000000"とする
    // Serial.println(Ps3_Data);

    // それぞれの変数にPs3のデータを代入
    left_X = Ps3_Data.substring(0, 4).toInt();
    left_Y = Ps3_Data.substring(5, 9).toInt();
    right_X = Ps3_Data.substring(10, 14).toInt();
    right_Y = Ps3_Data.substring(15, 19).toInt();

    SW_left = Ps3_Data.charAt(20) - '0';
    SW_right = Ps3_Data.charAt(21) - '0';
    SW_up = Ps3_Data.charAt(22) - '0';
    SW_down = Ps3_Data.charAt(23) - '0';

    SW_square = Ps3_Data.charAt(24) - '0';
    SW_circle = Ps3_Data.charAt(25) - '0';
    SW_triangle = Ps3_Data.charAt(26) - '0';
    SW_cross = Ps3_Data.charAt(27) - '0';

    SW_left1 = Ps3_Data.charAt(28) - '0';
    SW_left2 = Ps3_Data.charAt(29) - '0';
    SW_right1 = Ps3_Data.charAt(30) - '0';
    SW_right2 = Ps3_Data.charAt(31) - '0';

    SW_select = Ps3_Data.charAt(32) - '0';
    SW_start = Ps3_Data.charAt(33) - '0';

    // AnalogStickの角度を計算
    LStick_Angle = degrees(atan2(left_Y, left_X)) + 180;
    LStick_Dist = sqrt(float(left_X * left_X + left_Y * left_Y));
    if (LStick_Dist <= L_adjust)
        LStick_Angle = -1.0;

    RStick_Angle = degrees(atan2(right_Y, right_X)) + 180;
    RStick_Dist = sqrt(float(right_X * right_X + right_Y * right_Y));
    if (RStick_Dist <= R_adjust)
        RStick_Angle = -1.0;

    // AnalogStickで操作できるようにする
    SWleftStick_up = (left_Y < -100);
    SWleftStick_down = (left_Y > 100);
    SWleftStick_right = (left_X > 100);
    SWleftStick_left = (left_X < -100);

    SWrightStick_up = (right_Y < -100);
    SWrightStick_down = (right_Y > 100);
    SWrightStick_right = (right_X > 100);
    SWrightStick_left = (right_X < -100);

    Old_Ps3_Data = Ps3_Data;
}