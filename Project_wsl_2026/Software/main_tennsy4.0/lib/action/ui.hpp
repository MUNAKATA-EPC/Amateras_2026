#ifndef UI_HPP
#define UI_CPP

#include <Arduino.h>
#include "SSD1306.hpp"
#include "button.hpp"
#include "toggle.hpp"
#include "timer.hpp"

// 使うButtonの実態のみ定義
extern Button lcd_enter_button;
extern Button lcd_right_button;
extern Button lcd_left_button;

// LCDを動かすButtonのピンを定義
void ui_set_lcdpin(int enter_pin, int enter_pinmode, int left_pin, int left_pinmode, int right_pin, int right_pinmode);

// uiで使うButtonの設定
void ui_init();

// uiを実行する（選んでもらう）
void ui_mode_select();

// 攻撃か守備かテストかを出力
int get_ui_action();
/* ↓ 出力 */
// 攻撃モード
#define ACTION_ATTACKER_MODE 0
// 守備モード
#define ACTION_DEFENDER_MODE 1
// テスト(確認)モード
#define ACTION_TEST_MODE 2

// ジャイロのみかカメラを使うのかを出力
int get_ui_pd_mode();
/* ↓ 出力 */
// ジャイロモード
#define PD_GYRO_MODE 0
// カメラモード
#define PD_CAM_MODE 1

#endif