#ifndef UI_HPP
#define UI_CPP

#include <Arduino.h>
#include "SSD1306.hpp"
#include "button.hpp"
#include "toggle.hpp"
#include "timer.hpp"

// LCDを動かすButtonのピンを定義
void ui_set_lcdpin(int enter_pin, int enter_pinmode, int left_pin, int left_pinmode, int right_pin, int right_pinmode);

// uiで使うButtonの設定
void ui_init();

// uiを実行する（選んでもらう）
void ui_select();

// 攻撃か守備かテストかを出力
int get_selected_ui_action();
/* ↓ 出力 */
// 攻撃モード
#define ACTION_ATTACKER_MODE 0
// 守備モード
#define ACTION_DEFENDER_MODE 1
// テスト(確認)モード
#define ACTION_TEST_MODE 2

// ジャイロのみかカメラを使うのかどうか
bool is_selected_ui_use_cam();

// ジャイロのみかカメラを使うのかを出力
int get_selected_ui_test_mode();

#endif