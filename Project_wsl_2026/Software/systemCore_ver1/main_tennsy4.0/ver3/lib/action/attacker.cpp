#include "attacker.hpp"

inline PD *pdGyro = new PD(0.6, 0.1); // ジャイロ用のPD調節値
inline PD *pdCam = new PD(0.6, 0.1);  // カメラ用のPD調節値

void playAttacker() {
    motorsPdProcess(pdGyro, bnoDeg(), 0);

    motorsMove(0, 50); // 前進
}