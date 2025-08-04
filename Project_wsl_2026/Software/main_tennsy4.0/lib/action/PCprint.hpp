#ifndef PCPRINT_HPP
#define PCPRINT_HPP

// IRについて
#include "IRserial.hpp"
#include "IRcompute.hpp"
// LINEについて
#include "LINEserial.hpp"
#include "LINEcompute.hpp"
// BNO055について
#include "BNO055.hpp"
// OpenMVについて
#include "OpenMV.hpp"
// DSR1202について
#include "DSR1202.hpp"
#include "motors-boost_ver.hpp"
// キッカーについて
#include "kicker.hpp"

// パソコンにシリアルプリント用のプログラムを実行
void play_PCprint(int PCprint_mode);
/* ↓ 入力 */
// シリアルですべてのセンサーの状況を確認する
#define TEST_ALL_CHECK_WITH_SERIAL 0
// シリアルですべてのIRの状況を確認する
#define TEST_IR_CHECK_WITH_SERIAL 1
// シリアルですべてのLINEの状況を確認する
#define TEST_LINE_CHECK_WITH_SERIAL 2
// シリアルですべてのジャイロの状況を確認する
#define TEST_GYRO_CHECK_WITH_SERIAL 3
// シリアルですべてのカメラの状況を確認する
#define TEST_CAM_CHECK_WITH_SERIAL 4

#endif