#include "radicon.hpp"

static PD pdGyro(0.8, 0.1); // ジャイロ用のPD調節値
static PD pdCam(0.6, 0.1);  // カメラ用のPD調節値

void playRadicon()
{
    Radicon::Mode mode = (Radicon::Mode)uiModeNumber();

    // PD制御
    int pdDeg = bnoDeg();
    if (ps3RightStickDetected())
    {
        motorsPdProcess(&pdGyro, pdDeg, ps3RightStickDeg()); // PD成分計算(右ステック補正付き)
    }
    else
    {
        motorsPdProcess(&pdGyro, pdDeg, 0); // PD成分計算
    }

    // パワー！
    int power = 95;
    switch (mode)
    {
    case Radicon::SPEED_50CC:
        power = 40;
    case Radicon::SPEED_100CC:
        power = 65;
    case Radicon::SPEED_200CC:
        power = 95;
    default:
        power = 95;
        break;
    }

    // 制御
    if (ps3LeftStickDetected())
    {
        int moveDeg = (ps3LeftStickDeg() + bnoDeg() + 360) % 360; // 移動方向
        double scale = ps3LeftStickDis() / 128.0;                 // 速度のスケール計算

        motorsMove(moveDeg, (int)power * scale);
    }
    else
    {
        motorsPdMove();
    }

    // キッカー
    bool on = (catchSensor.read() == HIGH) || ps3ButtonIsPushing(ButtonDataType::L1) || ps3ButtonIsPushing(ButtonDataType::R1);
    kicker1.kick(on);
}