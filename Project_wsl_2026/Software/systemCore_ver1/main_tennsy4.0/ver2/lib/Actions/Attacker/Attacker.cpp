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
    if (line.detected())
    {
        motors.move(line.degMemory() + 180, power);
    }
    else if (ir.detected())
    {
        if (ir.deg() <= 10 || ir.deg() >= 350)
        {
            attack_deg = 0;
        }
        else if (ir.deg() <= 30 || ir.deg() >= 330)
        {
            attack_deg = mapDeg(ir.deg(), 60, 84, MapMode::HIREI);
        }
        else if (ir.val() > 50)
        {
            double hirei_val = ir.val() * 0.23;
            if (ir.deg() < 180)
            {
                attack_deg = (int)round(ir.deg() + hirei_val + 360) % 360;
            }
            else
            {
                attack_deg = (int)round(ir.deg() - hirei_val + 360) % 360;
            }
        }
        else
        {
            attack_deg = ir.deg();
        }

        motors.move(attack_deg, power);
    }
    else
    {
        motors.PDmove();
    }
}