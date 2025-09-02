#ifndef PS3_COMPUTE_HPP
#define PS3_COMPUTE_HPP

#include <Arduino.h>
#include "Ps3.hpp"

// 更新
void Ps3_compute_update();

// 上ボタンの状況を取得
bool get_Ps3_button_up();
// 下ボタンの状況を取得
bool get_Ps3_button_down();
// 左ボタンの状況を取得
bool get_Ps3_button_left();
// 右ボタンの状況を取得
bool get_Ps3_button_right();
// 三角ボタンの状況を取得
bool get_Ps3_button_triangle();
// 丸ボタンの状況を取得
bool get_Ps3_button_circle();
// バツボタンの状況を取得
bool get_Ps3_button_cross();
// 四角ボタンの状況を取得
bool get_Ps3_button_square();
// L1ボタンの状況を取得
bool get_Ps3_button_l1();
// L2ボタンの状況を取得
bool get_Ps3_button_l2();
// L3ボタンの状況を取得
bool get_Ps3_button_l3();
// R1ボタンの状況を取得
bool get_Ps3_button_r1();
// R2ボタンの状況を取得
bool get_Ps3_button_r2();
// R3ボタンの状況を取得
bool get_Ps3_button_r3();

// 左スティックの角度を取得
int get_Ps3_stick_left_deg();
// 左スティックの距離を取得
int get_Ps3_stick_left_distance();
// 右スティックの角度を取得
int get_Ps3_stick_right_deg();
// 右スティックの距離を取得
int get_Ps3_stick_right_distance();

#endif