#ifndef LINECOMPUTE_HPP
#define LINECOMPUTE_HPP

#include <Arduino.h>
#include "LINE.hpp"

// 更新
void LINE_compute_update();

// 右サイドの状況を取得
bool get_LINE_side_right();
// 左サイドの状況を取得
bool get_LINE_side_left();
// 後サイドの状況を取得
bool get_LINE_side_back();

// エンジェルライン＋サイドでの角度を取得
int get_LINE_deg();

// 最初に反応したピンを取得
int get_first_triggered_pin();

// ハーフアウト検知
bool is_LINE_half_out();

#endif