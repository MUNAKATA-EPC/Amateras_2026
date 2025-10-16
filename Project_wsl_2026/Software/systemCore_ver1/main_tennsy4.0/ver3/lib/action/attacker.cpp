#include "attacker.hpp"

void playAttacker() {
    motorsPdProcess(pdGyro, bnoDeg(), 0);

    motorsMove(0, 50); // 前進
}