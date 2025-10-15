#include "Defender/Defender.hpp"

void playDefender(int power)
{
    Defender::Mode mode = (Defender::Mode)ui.modeNumber();

    // PD制御
    int pd_deg = 0;
    PD *pd = &pdGyro; // デフォルトはジャイロ
    pd_deg = bno.deg();
    motors.PDprocess(pd, pd_deg, 0); // PD成分計

    // ジャイロモードなら攻撃する
    if (mode == Defender::Mode::GYRO)
    {
        playAttacker(power);
        return;
    }

    // 守備の動きを計算
    if (line.detected(LineSensor::RING))
    {
        Serial.println(line.x());

        Vector defenceVec(line.x(), 0, true);

        motors.move(defenceVec.deg(), power);
    }
    else
    {
        motors.PDmove();
    }
}
