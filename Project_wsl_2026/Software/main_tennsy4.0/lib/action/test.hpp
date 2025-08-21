#ifndef TEST_HPP
#define TEST_HPP

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

// 動作確認用のプログラムを実行
void play_test(uint8_t test_mode);

#endif