#ifndef DEFENDER_HPP
#define DEFENDER_HPP

// IRについて
#include "IR.hpp"
// LINEについて
#include "LINE.hpp"
// BNO055について
#include "Gyro.hpp"
// OpenMVについて
#include "Camera.hpp"
// DSR1202について
#include "Motors.hpp"
// キッカーについて
#include "kicker.hpp"
// PS3コントローラーについて
#include "Ps3.hpp"
// uiについて
#include "ui.hpp"
// 守備用のプログラムを実行
void play_defender(bool use_yellow_cam, bool use_blue_cam, int16_t motor_power);

#endif