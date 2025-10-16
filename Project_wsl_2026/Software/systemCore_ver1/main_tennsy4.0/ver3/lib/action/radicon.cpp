#include "radicon.hpp"

inline PD *pdGyro = new PD(0.6, 0.1); // ジャイロ用のPD調節値
inline PD *pdCam = new PD(0.6, 0.1);  // カメラ用のPD調節値

void playRadicon()
{
    Radicon::Mode mode = (Radicon::Mode)uiModeNumber();

    // PD制御
    int pdDeg = bnoDeg();
    if (ps3Detected(StickDataType::RIGHTSTICK))
    {
        motorsPdProcess(pdGyro, pdDeg, ps3Deg(StickDataType::RIGHTSTICK)); // PD成分計算(右ステック補正付き)
    }
    else
    {
        motorsPdProcess(pdGyro, pdDeg, 0); // PD成分計算
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
    }

    // 制御
    if (ps3Detected(StickDataType::LEFTSTICK))
    {
        int moveDeg = (ps3Deg(StickDataType::LEFTSTICK) + bnoDeg() + 360) % 360; // 移動方向
        double scale = ps3Dis(StickDataType::LEFTSTICK) / 128.0;                 // 速度のスケール計算

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