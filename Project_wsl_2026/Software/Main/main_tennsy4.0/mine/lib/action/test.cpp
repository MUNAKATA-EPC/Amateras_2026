#include "test.hpp"

void testKicker();
void testGyro();
void testCam();
void testLineTrace();

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
        kicker1.kick(true);
        fullColorLed1.rgbLightUp(0, 50, 0);
    }
    else if (catchSensor.read() == HIGH)
    {
        Serial.print("kick");
        kicker1.kick(true);
        fullColorLed1.rgbLightUp(0, 50, 0);
    }
    else
    {
        kicker1.kick(false);
        fullColorLed1.rgbLightUp(0, 0, 0);
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
        if ((lineRingDis() > 60) && abs(diffDeg(lineRingDeg(), irDeg())) < 90)
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