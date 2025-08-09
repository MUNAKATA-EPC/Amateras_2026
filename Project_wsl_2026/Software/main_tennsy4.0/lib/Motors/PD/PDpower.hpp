#ifndef PDPOWER_HPP
#define PDPOWER_HPP

#include <Arduino.h>
#include "Motors.hpp"

// ジャイロをPD制御で使う場合
void PD_use_gyro();
void PD_use_gyro_set_target_ver(int target_deg); // 目標角度が自由に設定できるバージョン
// カメラによる角度をPD制御で使う場合
void PD_use_yellow_goal(); // 黄色ゴール
void PD_use_blue_goal();   // 青ゴール

// 算出したPD出力を取得
int get_PD_power();

#endif