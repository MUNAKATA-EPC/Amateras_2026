#include "Radicon/Radicon.hpp"

PD pdGyro(0.6, 0.02); // ジャイロ用のPD

void playRadicon(Radicon::Mode mode, int power)
{
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

    // キッカー
    bool on = (catchSensor.read() == HIGH) || ps3.button(M5stamp::L1) || ps3.button(M5stamp::R1);
    kicker.kick(on);

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
}