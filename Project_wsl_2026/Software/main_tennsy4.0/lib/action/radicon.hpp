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
// uiについて
#include "ui.hpp"

// ラジコン用のプログラムを実行
void play_radicon(int radicon_mode);

#endif