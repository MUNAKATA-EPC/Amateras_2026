#ifndef ATTACKER_HPP
#define ATTACKER_HPP

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
// uiについて
#include "ui.hpp"

// 攻撃用のプログラムを実行
void play_attacker(bool use_cam, int motor_power);

#endif