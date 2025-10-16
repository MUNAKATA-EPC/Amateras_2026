#include "radicon.hpp"

inline PD *pdGyro = new PD(0.6, 0.1); // ジャイロ用のPD調節値
inline PD *pdCam = new PD(0.6, 0.1);  // カメラ用のPD調節値

void playRadicon() {
    Radicon::Mode mode = (Radicon::Mode)uiModeNumber();

    // PD制御
    int pdDeg = bnoDeg();
    if (m5stampDetected(StickDataType::RIGHTSTICK))
    {
        motorsPdProcess(pdGyro, pdDeg, m5stampDeg(StickDataType::RIGHTSTICK)); // PD成分計算(右ステック補正付き)
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
    if (m5stampDetected(StickDataType::LEFTSTICK))
    {
        int moveDeg = (m5stampDeg(StickDataType::LEFTSTICK) + bnoDeg() + 360) % 360; // 移動方向
        double scale = m5stampDis(StickDataType::LEFTSTICK) / 128.0;                   // 速度のスケール計算

        motorsMove(moveDeg, (int)power * scale);
    }
    else
    {
        motorsPdMove();
    }

    // キッカー
    bool on = (catchSensor.read() == HIGH) || m5stampButtonIsPushing(ButtonDataType::L1) || m5stampButtonIsPushing(ButtonDataType::R1);
    kicker1.kick(on);
}