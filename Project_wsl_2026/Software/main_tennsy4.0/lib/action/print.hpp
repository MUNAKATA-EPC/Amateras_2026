#ifndef PRINT_HPP
#define PRINT_HPP

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
// SSD1306について
#include "SSD1306.hpp"
// uiについて
#include "ui.hpp"

// パソコンにシリアルプリント用のプログラムを実行
void play_pc_print(int pc_print_mode);
/* ↓ 入力 */
// シリアルですべてのセンサーの状況を確認する
#define ALL_CHECK_WITH_PC 0
// シリアルですべてのIRの状況を確認する
#define IR_CHECK_WITH_PC 1
// シリアルですべてのLINEの状況を確認する
#define LINE_CHECK_WITH_PC 2
// シリアルですべてのジャイロの状況を確認する
#define GYRO_CHECK_WITH_PC 3
// シリアルですべてのカメラの状況を確認する
#define CAM_CHECK_WITH_PC 4

// LCDにプリント用のプログラムを実行＊clearとshowは外部処理＊
void play_lcd_print(int lcd_print_mode, int ahead_x, int ahead_y);
/* ↓ 入力 */
// LCDでIRとLINEとジャイロの状況を確認する
#define IR_LINE_GYRO_CHECK_WITH_LCD 0
// LCDですべてのIRの状況を確認する
#define IR_CHECK_WITH_LCD 1
// LCDですべてのLINEの状況を確認する
#define LINE_CHECK_WITH_LCD 2
// LCDですべてのジャイロの状況を確認する
#define GYRO_CHECK_WITH_LCD 3
// LCDですべてのカメラの状況を確認する
#define CAM_CHECK_WITH_LCD 4

#endif