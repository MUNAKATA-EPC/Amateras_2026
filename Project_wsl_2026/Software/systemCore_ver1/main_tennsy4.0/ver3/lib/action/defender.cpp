#include "defender.hpp"

void playDefender() {
    motorsPdProcess(pdGyro, bnoDeg(), 0);

    motorsMove(0, 50); // 前進
}