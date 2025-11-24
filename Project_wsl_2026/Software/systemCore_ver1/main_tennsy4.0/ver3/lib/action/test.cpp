#include "test.hpp"

static PD pdGyro(0.4f, 1.6f); // ジャイロ用のPD調節値
static PD pdCam(0.9f, 1.1f);  // カメラ用のPD調節値

static Timer timer;

void playTest()
{
    /*
    motorsPdProcess(&pdGyro, bnoDeg(), 0);

    if (!timer.everCalled() || timer.msTime() > 4000)
        timer.reset();

    if (timer.msTime() < 1000)
    {
        motorsMove(90, 60);
    }
    else if (timer.msTime() < 2000)
    {
        motorsMove(0, 60);
    }
    else if (timer.msTime() < 3000)
    {
        motorsMove(-90, 60);
    }
    else
    {
        motorsMove(-180, 60);
    }
    */

    if (resetButton.isReleased())
    {
        kicker1.kick(true);
    }
    else
    {
        kicker1.kick(false);
    }
}