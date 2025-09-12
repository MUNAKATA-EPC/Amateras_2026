#include "Attacker/Attacker.hpp"

PD pdGyro(0.6, 0.02); // ジャイロ用のPD

void playAttacker(Attacker::Mode mode, int power)
{
    // PD制御
    int pd_deg = bno.deg();
    PD *pd = &pdGyro;                // デフォルトはジャイロ
    motors.PDprocess(pd, pd_deg, 0); // PD成分計算

    // キッカー
    kicker.kick(catchSensor.read() == HIGH);

    // 制御
}