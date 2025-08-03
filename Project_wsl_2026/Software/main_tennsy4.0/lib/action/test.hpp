#ifndef TEST_HPP
#define TEST_HPP

/*テスト用のプログラムを実行する*/

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
#include "motors.hpp"

// モード
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

// テスト用のプログラムを実行
void play_test(int test_mode);

#endif