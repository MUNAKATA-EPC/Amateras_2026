#include "Attacker/Attacker.hpp"

PD pdGyro(0.78, 0.025); // ジャイロ用のPD
PD pdCam(0.6, 0.02);    // カメラ用のPD

void playAttacker(int mode, int power)
{
    // PD制御
    int pd_deg = 0;
    PD *pd = &pdGyro; // デフォルトはジャイロ
    if (mode == Attacker::Mode::YELLOWGOAL && cam.yellowGoal.detected())
    {
        pd = &pdCam;
        pd_deg = cam.yellowGoal.deg();
    }
    else if (mode == Attacker::Mode::BLUEGOAL && cam.blueGoal.detected())
    {
        pd = &pdCam;
        pd_deg = cam.blueGoal.deg();
    }
    else
    {
        pd_deg = bno.deg();
    }
    motors.PDprocess(pd, pd_deg, 0); // PD成分計算

    // キッカー
    kicker.kick(catchSensor.readCatch() == DigitalSensor::CatchSensor::Catch);

    if(area4(line.deg()) == Area4::BACK)

    // 移動
    motors.move(0, 50);
}