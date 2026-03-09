#include <Arduino.h>
#include "MySettings.h"
#include "M5STAMPps3.h"
#include "M5STAMPcommunicate.h"

/*ps3*/

int Left_X = 0, Left_Y = 0, Right_X = 0, Right_Y = 0;
bool SW_up = false, SW_down = false, SW_right = false, SW_left = false;
bool SW_triangle = false, SW_cross = false, SW_circle = false, SW_square = false;
bool SW_left1 = false, SW_left2 = false, SW_right1 = false, SW_right2 = false;
bool SWleftStick_left = false, SWleftStick_right = false, SWleftStick_up = false, SWleftStick_down = false;
bool SWrightStick_left = false, SWrightStick_right = false, SWrightStick_up = false, SWrightStick_down = false;
float LStick_Angle = 0.0, LStick_Dist = 0.0, RStick_Angle = 0.0, RStick_Dist = 0.0;
int L_adjust = L_ADJUSTNUM, R_adjust = R_ADJUSTNUM;

String M5STAMPps3data = "a0000";

void M5STAMPps3begin()
{
    M5STAMPPS3_SERIAL.begin(M5STAMPPS3_SERIALSPEED);
}

void M5STAMPps3get()
{
    while (M5STAMPPS3_SERIAL.available())
    {
        M5STAMPps3data = M5STAMPPS3_SERIAL.readStringUntil('\n'); // データを読み取る
        M5STAMPps3data.trim();                                    // \rや空白を消去

        // 受信したデータが5文字なら正常なデータである
        if (M5STAMPps3data.length() == 5)
        {
            if (M5STAMPps3data.charAt(0) == 'a')
            {
                Left_X = M5STAMPps3data.substring(2, 5).toInt();
                if (M5STAMPps3data.charAt(1) == '-')
                    Left_X *= -1;
            }
            else if (M5STAMPps3data.charAt(0) == 'b')
            {
                Left_Y = M5STAMPps3data.substring(2, 5).toInt();
                if (M5STAMPps3data.charAt(1) == '-')
                    Left_Y *= -1;
            }
            else if (M5STAMPps3data.charAt(0) == 'c')
            {
                Right_X = M5STAMPps3data.substring(2, 5).toInt();
                if (M5STAMPps3data.charAt(1) == '-')
                    Right_X *= -1;
            }
            else if (M5STAMPps3data.charAt(0) == 'd')
            {
                Right_Y = M5STAMPps3data.substring(2, 5).toInt();
                if (M5STAMPps3data.charAt(1) == '-')
                    Right_Y *= -1;
            }
            else if (M5STAMPps3data.charAt(0) == 'e')
            {
                SW_up = M5STAMPps3data.charAt(1) - '0';
                SW_down = M5STAMPps3data.charAt(2) - '0';
                SW_right = M5STAMPps3data.charAt(3) - '0';
                SW_left = M5STAMPps3data.charAt(4) - '0';
            }
            else if (M5STAMPps3data.charAt(0) == 'f')
            {
                SW_triangle = M5STAMPps3data.charAt(1) - '0';
                SW_cross = M5STAMPps3data.charAt(2) - '0';
                SW_circle = M5STAMPps3data.charAt(3) - '0';
                SW_square = M5STAMPps3data.charAt(4) - '0';
            }
            else if (M5STAMPps3data.charAt(0) == 'g')
            {
                SW_left1 = M5STAMPps3data.charAt(1) - '0';
                SW_left2 = M5STAMPps3data.charAt(2) - '0';
                SW_right1 = M5STAMPps3data.charAt(3) - '0';
                SW_right2 = M5STAMPps3data.charAt(4) - '0';
            }
        }
    }

    // AnalogStickの角度を計算
    LStick_Angle = degrees(atan2(Left_Y, Left_X)) + 180;
    LStick_Angle = (int(LStick_Angle + 270)) % 360;
    LStick_Dist = sqrt(float(Left_X * Left_X + Left_Y * Left_Y));
    if (LStick_Dist <= L_adjust)
        LStick_Angle = -1.0;

    RStick_Angle = degrees(atan2(Right_Y, Right_X)) + 180;
    RStick_Angle = (int(RStick_Angle + 270)) % 360;
    RStick_Dist = sqrt(float(Right_X * Right_X + Right_Y * Right_Y));
    if (RStick_Dist <= R_adjust)
        RStick_Angle = -1.0;

    // AnalogStickで操作できるようにする
    SWleftStick_up = (Left_Y < -100);
    SWleftStick_down = (Left_Y > 100);
    SWleftStick_right = (Left_X > 100);
    SWleftStick_left = (Left_X < -100);

    SWrightStick_up = (Right_Y < -100);
    SWrightStick_down = (Right_Y > 100);
    SWrightStick_right = (Right_X > 100);
    SWrightStick_left = (Right_X < -100);
}

/*communicate*/

void M5STAMPcommunicatebegin()
{
    M5STAMPCOMMUNICATE_SERIAL.begin(M5STAMPCOMMUNICATE_SERIALSPEED);
}

void M5STAMPcommunicateget()
{
}