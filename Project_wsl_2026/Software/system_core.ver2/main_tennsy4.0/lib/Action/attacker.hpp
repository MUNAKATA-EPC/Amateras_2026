ifndef ATTACKER_HPP
#define ATTACKER_HPP

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

// 攻撃用のプログラムを実行
void play_attacker(bool use_yellow_cam, bool use_blue_cam, int motor_power);

// 角度における"差"を計算する関数
int compute_deg_diff(int a, int b);

#endif