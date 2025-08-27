#ifndef LINECOMPUTE_HPP
#define LINECOMPUTE_HPP

#include <Arduino.h>
#include "LINE.hpp"

// 角度計算
void LINE_compute_update();

// サイドの状況を取得
bool get_LINE_side_right(); // 右サイドの状況を取得
bool get_LINE_side_left();  // 左サイドの状況を取得
bool get_LINE_side_back();  // 後サイドの状況を取得

// ラインの角度を取得
int get_LINE_memory_deg();

#endif
