#ifndef RADICON_HPP
#define RADICON_HPP

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

// ラジコン用のプログラムを実行
void play_radicon(int16_t motor_power);

#endif