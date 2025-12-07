#include "attacker.hpp"

static PD pdGyro(0.55, 1.6); // ジャイロ用のPD調節値

void playAttacker(Attacker::Mode mode)
{
    motorsPdProcess(&pdGyro, bnoDeg(), 0); // ジャイロで姿勢制御
    motorsPdMove();
}