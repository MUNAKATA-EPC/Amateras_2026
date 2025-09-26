#include "Actions.hpp"

PD pdGyro(0.6, 0.02); // ジャイロ用のPD
PD pdCam(0.6, 0.02);  // カメラ用のPD