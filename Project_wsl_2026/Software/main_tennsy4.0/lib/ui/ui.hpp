#ifndef UI_HPP
#define UI_CPP

#include <Arduino.h>
#include "button.hpp"
#include "toggle.hpp"
#include "timer.hpp"

// uiを実行する（選んでもらう）
void ui_process();

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