#include "Attacker/Attacker.hpp"

PD pdGyro(0.78, 0.025); // ジャイロ用のPD
PD pdCam(0.6, 0.02);    // カメラ用のPD

void playAttacker(int mode, int power)
{
    // PD制御
    switch (mode)
    {
    case Attacker::Mode::GYRO:
        motors.PDprocess(&pdGyro, bno.deg(), 0);
        break;
    case Attacker::Mode::YELLOWGOAL:
        if (cam.yellowGoal.detected())
        {
            motors.PDprocess(&pdCam, cam.yellowGoal.deg(), 0);
        }
        else
        {
            motors.PDprocess(&pdGyro, bno.deg(), 0);
        }
        break;
    case Attacker::Mode::BLUEGOAL:
        if (cam.blueGoal.detected())
        {
            motors.PDprocess(&pdCam, cam.blueGoal.deg(), 0);
        }
        else
        {
            motors.PDprocess(&pdGyro, bno.deg(), 0);
        }
        break;
    }

    //キッカー
    kicker.kick(catchSensor.read() == CATCH);

    //移動
    motors.move(0, 50);
}