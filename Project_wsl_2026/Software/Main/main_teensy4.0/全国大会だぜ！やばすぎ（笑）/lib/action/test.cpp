#include "test.hpp"

static PD pd_gyro(0.50f, -3.0f);
static PD pd_cam(0.50f, -3.0f);

void testKicker();
void testGyro();
void testCam();
void testLineTrace();
void testMotor();
void testBallDisAdjust();

void playTest(Test::Mode mode)
{
    if (mode == Test::Mode::KICKER)
    {
        testKicker();
    }
    else if (mode == Test::Mode::GYRO)
    {
        testGyro();
    }
    else if (mode == Test::Mode::CAM)
    {
        testCam();
    }
    else if (mode == Test::Mode::LINE_TRACE)
    {
        testLineTrace();
    }
    else if (mode == Test::Mode::MOTOR)
    {
        testMotor();
    }
    else if (mode == Test::Mode::BALL_DIS_ADJUST)
    {
        testBallDisAdjust();
    }
    else
    {
        motorsStop();
    }
}

void testKicker()
{
    if (resetButton.isReleased())
    {
        Serial.print("kick");
        kicker1.kick();
        // fullColorLed1.rgbLightUp(0, 50, 0);
    }
    else if (catchSensor.read() == HIGH)
    {
        Serial.print("kick");
        kicker1.kick();
        // fullColorLed1.rgbLightUp(0, 50, 0);
    }
    else
    {
        // fullColorLed1.rgbLightUp(0, 0, 0);
    }
}

void testGyro()
{
    motorsPdProcess(&pd_gyro, bnoDeg(), 0); // PD成分計算
    motorsPdMove();
}

void testCam()
{
    int pd_deg = 0;

    if (yellowGoalDetected())
    {
        pd_deg = yellowGoalDeg();
    }
    else if (blueGoalDetected())
    {
        pd_deg = blueGoalDeg();
    }
    else
    {
        pd_deg = bnoDeg();
    }

    motorsPdProcess(&pd_cam, pd_deg, 0); // PD成分計算
    motorsPdMove();
}

void testLineTrace()
{
    motorsPdProcess(&pd_gyro, bnoDeg(), 0); // ジャイロで姿勢制御

    const int motor_ir_power = 60;
    const int motor_line_escape_power = 60;
    const int motor_line_trace_power = 30;

    if (lineRingDetected())
    {
        if ((lineRingDis() > 60) && fabsf(diffDeg(lineRingDeg(), irDeg())) < 90)
        {
            motorsMove(nearSessenDeg(lineRingDeg(), irDeg()), motor_line_trace_power);
        }
        else
        {
            motorsMove(lineRingDeg() + 180, motor_line_escape_power);
        }
    }
    else
    {
        motorsMove(irDeg(), motor_ir_power);
    }
}

void testMotor()
{
    motorsDirectMove(-100, 100, -100, 100);
}

void testBallDisAdjust()
{
    static String mode = "reset";
    static int deg_to_dis[360];

    while (Serial.available())
    {
        mode = Serial.readStringUntil('\n');
    }

    static bool flag = true;

    if (mode == "home")
    {
        if (flag)
        {
            Serial.print(mode + " : ");
            Serial.println("home:セレクト,reset:データリセット,show:データ表示,collect:データ採取");
            flag = false;
        }
    }
    else if (mode == "reset")
    {
        for (int i = 0; i < 360; i++)
        {
            deg_to_dis[i] = 1023;
        }
        Serial.print(mode + " : ");
        Serial.println("success");

        mode = "home";
        flag = true;
    }
    else if (mode == "show")
    {
        Serial.print(mode + " : ");
        Serial.print("{");
        for (int i = 0; i < 360; i++)
        {
            Serial.print(String(deg_to_dis[i]) + ",");
        }
        Serial.println("}");

        mode = "home";
        flag = true;
    }
    else if (mode == "collect")
    {
        Serial.print(mode + " : ");
        if (irDetected())
        {
            int index = ((int)irDeg() + 360) % 360;
            int dis = irDis();

            if (dis < deg_to_dis[index])
            {
                deg_to_dis[index] = dis;
            }

            Serial.println(dis);
        }
        else
        {
            Serial.println("0xFF");
        }

        flag = true;
    }
    else
    {
        Serial.print(mode + " : ");
        Serial.println("error");

        mode = "home";
        flag = true;
    }
}