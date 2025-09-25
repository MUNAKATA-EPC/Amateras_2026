#include "Attacker/Attacker.hpp"

PD pdGyro(0.6, 0.02); // ジャイロ用のPD
PD pdCam(0.6, 0.02);  // カメラ用のPD

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
        if (ir.deg() < 10 || ir.deg() > 350)
        {
            motors.move(0, power);
        }
        else if (ir.deg() < 25 || ir.deg() > 335)
        {
            motors.move(mapDeg(ir.deg(), 30, 48, MapMode::HIREI), power);
        }
        else if (ir.val() > 60)
        {
            double hirei_val = ir.val() * 0.226;
            int mawarikomi_deg;

            if (ir.deg() < 180)
                mawarikomi_deg = (int)round(ir.deg() + hirei_val + 360) % 360;
            else
                mawarikomi_deg = (int)round(ir.deg() - hirei_val + 360) % 360;

            motors.move(mawarikomi_deg, power);
        }
        else
        {
            motors.move(ir.deg(), power);
        }
    }
    else
    {
        motors.PDmove();
    }
}