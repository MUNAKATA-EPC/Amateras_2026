#ifndef PDPOWER_HPP
#define PDPOWER_HPP

#include <Arduino.h>
#include "Motors.hpp"

// ジャイロをPD制御で使う場合
void PD_use_gyro(int16_t target_deg);

// カメラによる角度をPD制御で使う場合
// 黄色ゴール
void PD_use_yellow_goal();
// 青ゴール
void PD_use_blue_goal();

//  算出したPD出力を取得
int16_t get_PD_power();

#endif
