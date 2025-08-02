#ifndef PDPOWER_HPP
#define PDPOWER_HPP

/*PD制御の出力値を計算する*/

#include <Arduino.h>
#include "BNO055.hpp"
#include "OpenMV.hpp"

// ジャイロをPD制御で使う場合
void PD_use_gyro();
// カメラによる角度をPD制御で使う場合
void PD_use_cam();

// 算出したPD出力を取得
int get_PD_power();

#endif