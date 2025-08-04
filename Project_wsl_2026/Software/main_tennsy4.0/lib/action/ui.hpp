#ifndef UI_HPP
#define UI_CPP

#include <Arduino.h>
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
// LCDについて
#include "SSD1306.hpp"
// 便利な関数♡
#include "button.hpp"
#include "toggle.hpp"
#include "timer.hpp"

// LCDを動かすButtonのピンを定義
void ui_set_lcdpin(int enter_pin, int enter_pinmode, int left_pin, int left_pinmode, int right_pin, int right_pinmode);

// uiで使うButtonの設定
void ui_init();

// uiを実行する（選んでもらう）
void ui_process();

// まだ選ばれている途中かどうか
bool is_now_selecting_ui();

// 攻撃か守備かテストかを出力
int get_selected_ui_action();
/* ↓ 出力 */
// 攻撃する
#define ACTION_ATTACKER 0
// 守備する
#define ACTION_DEFENDER 1
// テスト(確認)する
#define ACTION_TEST 2

// 選ばれたActionの中でのモードを出力
int get_selected_ui_mode();
/* ↓ 出力 */
// 攻撃or守備をするとき
//  PD制御でジャイロだけ使うモード
#define PD_USE_ONLY_GYRO_MODE 0
// PD制御でカメラも使うモード
#define PD_USE_CAM_MODE 1

// テストをするとき
//  キッカーの動作を確認するモード
#define TEST_KICKER_MODE 0
// ジャイロでのPD制御のテストを行うモード
#define TEST_PD_GYRO_MODE 1
// カメラでのPD制御のテストを行うモード
#define TEST_PD_CAM_MODE 2

// LCDにプリントするプログラムを実行＊clearとshowは外部処理＊
void play_lcd_print(int lcd_print_mode);
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