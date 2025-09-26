#include "Radicon/Radicon.hpp"

void playRadicon(int power)
{
    Radicon::Mode mode = (Radicon::Mode)ui.modeNumber();

    // PD制御
    int pd_deg = bno.deg();
    PD *pd = &pdGyro; // デフォルトはジャイロ
    if (ps3.detected(M5stamp::RIGHTSTICK))
    {
        motors.PDprocess(pd, pd_deg, ps3.deg(M5stamp::RIGHTSTICK)); // PD成分計算(右ステック補正付き)
    }
    else
    {
        motors.PDprocess(pd, pd_deg, 0); // PD成分計算
    }

    // パワー！
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
    }

    // 制御
    if (ps3.detected(M5stamp::LEFTSTICK))
    {
        int move_deg = (ps3.deg(M5stamp::LEFTSTICK) + bno.deg() + 360) % 360;  // 移動方向
        double scale = map(ps3.dis(M5stamp::LEFTSTICK), 0.0, 128.0, 0.0, 1.0); // 速度のスケール計算

        motors.move(move_deg, (int)power * scale);
    }
    else
    {
        motors.PDmove();
    }

    // キッカー
    bool on = (catchSensor.read() == HIGH) || ps3.button(M5stamp::L1) || ps3.button(M5stamp::R1);
    kicker.kick(on);
}