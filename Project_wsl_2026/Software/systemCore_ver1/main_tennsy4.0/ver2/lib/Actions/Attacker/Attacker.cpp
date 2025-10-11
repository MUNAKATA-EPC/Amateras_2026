#include "Attacker/Attacker.hpp"

int attack_deg = 0;

void playAttacker(int power)
{
    Attacker::Mode mode = (Attacker::Mode)ui.modeNumber();

    // PD制御
    int pd_deg = 0;
    PD *pd = &pdGyro; // デフォルトはジャイロ
    if (mode == Attacker::Mode::YELLOWGOAL && cam.detected(Openmv::YELLOWGOAL))
    {
        pd = &pdCam;
        pd_deg = cam.deg(Openmv::YELLOWGOAL);
    }
    else if (mode == Attacker::Mode::BLUEGOAL && cam.detected(Openmv::BLUEGOAL))
    {
        pd = &pdCam;
        pd_deg = cam.deg(Openmv::BLUEGOAL);
    }
    else
    {
        pd_deg = bno.deg();
    }
    motors.PDprocess(pd, pd_deg, 0); // PD成分計算

    // キッカー
    kicker.kick(catchSensor.read() == HIGH);

    // 制御
}