#ifndef UI_HPP
#define UI_CPP

#include <Arduino.h>
#include "button.hpp"
#include "toggle.hpp"
#include "timer.hpp"
// LCDプリント用
#include "SSD1306.hpp"
#include "print.hpp"

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
// ラジコンする
#define ACTION_RADICON 3

// 選ばれたActionの中でのモードを出力
int get_selected_ui_mode();
/* ↓ 出力 */
// 攻撃or守備をするとき
//  PD制御でジャイロだけ使うモード
#define PD_USE_ONLY_GYRO_MODE 0
// PD制御で黄ゴールのカメラも使うモード
#define PD_USE_YELLOW_CAM_MODE 1
// PD制御で青ゴールのカメラも使うモード
#define PD_USE_BLUE_CAM_MODE 2

// テストをするとき
//  キッカーの動作を確認するモード
#define TEST_KICKER_MODE 0
// ジャイロでのPD制御のテストを行うモード
#define TEST_MONITOR_MODE 1
// カメラでのPD制御のテストを行うモード
#define TEST_PD_MODE 2

// ラジコンをするとき
//  キッカーの動作を確認するモード
#define RADICON_50cc_MODE 0
#define RADICON_100cc_MODE 1
#define RADICON_150cc_MODE 2
#define RADICON_200cc_MODE 3

#endif