#include "test.hpp"
static PD pdGyro(0.4f, 1.6f); // ジャイロ用のPD調節値
static PD pdCam(0.9f, 1.1f);  // カメラ用のPD調節値

void testKicker()
{
    if (resetButton.isReleased())
    {
        kicker1.kick(true);
    }
    else
    {
        kicker1.kick(false);
    }
}

void testGyro()
{
    motorsPdProcess(&pdGyro, bnoDeg(), 0); // PD成分計算
    motorsPdMove();
}

void testCam()
{
    int pd_deg = 0;

    if (yellowGoalDetected())
        pd_deg = yellowGoalDeg();
    else if (blueGoalDetected())
        pd_deg = blueGoalDeg();
    else
        pd_deg = bnoDeg();

    motorsPdProcess(&pdCam, pd_deg, 0); // PD成分計算
    motorsPdMove();
}



void playTest()
{
    Test::Mode mode = (Test::Mode)uiModeNumber();

    switch (mode)
    {
    case Test::Mode::KICKER:
        testKicker();
        break;
    case Test::Mode::GYRO:
        testGyro();
        break;
    case Test::Mode::CAM:
        testCam();
        break;
    default:
        motorsStop();
        break;
    }
}