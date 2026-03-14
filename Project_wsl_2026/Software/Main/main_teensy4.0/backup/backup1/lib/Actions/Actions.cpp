#include "Actions.hpp"

PD pdGyro(0.65, 0.025); // ジャイロ用のPD
PD pdCam(0.65, 0.025);  // カメラ用のPD