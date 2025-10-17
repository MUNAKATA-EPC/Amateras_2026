#include "defender.hpp"

static PD *pdGyro = new PD(0.6, 0.1); // ジャイロ用のPD調節値
static PD *pdCam = new PD(0.6, 0.1);  // カメラ用のPD調節値

void playDefender() {
    motorsPdProcess(pdGyro, bnoDeg(), 0);

    motorsMove(0, 50); // 前進
}