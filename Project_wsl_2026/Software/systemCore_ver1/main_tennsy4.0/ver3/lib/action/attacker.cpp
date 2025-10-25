#include "attacker.hpp"

static AnglePD pdGyro(0.8, 0.1); // ジャイロ用のPD調節値
static AnglePD pdCam(0.6, 0.1);  // カメラ用のPD調節値

static Timer camTimer;

void playAttacker()
{
    // キャッチしたらゴール方向を一定時間向く
    if (catchSensor.read() == HIGH)
        camTimer.reset();

    // PD制御
    if ((Attacker::Mode)uiModeNumber() == Attacker::Mode::GYRO)
    {
        motorsPdProcess(&pdGyro, bnoDeg(), 0);
    }
    else
    {
        if (yellowGoalDetected() && camTimer.msTime() < 1600 && camTimer.everCalled())
        {
            motorsPdProcess(&pdCam, yellowGoalDeg(), 0);
        }
        else
        {
            motorsPdProcess(&pdGyro, bnoDeg(), 0);
        }
    }

    // キッカー
    kicker1.kick(catchSensor.read() == HIGH);

    // 制御
    if (lineRingDetected())
    {
        int toEscapeDeg = lineRingDeg() + 180;

        if (abs(diffDeg(toEscapeDeg, fieldDeg())) > 90)
        {
            toEscapeDeg = toEscapeDeg + 180;
        }

        toEscapeDeg = (toEscapeDeg + 360) % 360;

        motorsMove(toEscapeDeg, 95);
    }
    else if (irDetected())
    {
        if (irDeg() < 8 || irDeg() > 352)
        {
            motorsMove(0, 90);
        }
        else if (irDeg() < 15 || irDeg() > 345)
        {
            int toHoldDeg = mapDeg(irDeg(), 20, 60, MapMode::HIREI);

            motorsMove(toHoldDeg, 60);
        }
        else
        {
            double diffMawarikomiDeg = irVal() / 5.2;
            int mawarikomiDeg = (irDeg() < 180) ? irDeg() + (int)diffMawarikomiDeg : irDeg() - (int)diffMawarikomiDeg;
            mawarikomiDeg = (mawarikomiDeg + 360) % 360;

            motorsMove(mawarikomiDeg, 90);
        }
    }
    else
    {
        motorsPdMove();
    }
}