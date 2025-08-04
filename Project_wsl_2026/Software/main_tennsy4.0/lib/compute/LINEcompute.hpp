#ifndef LINECOMPUTE_HPP
#define LINECOMPUTE_HPP

#include <Arduino.h>
#include "LINEserial.hpp"

// 右サイドの状況を取得
bool get_LINE_side_right();
// 左サイドの状況を取得
bool get_LINE_side_left();
// 後サイドの状況を取得
bool get_LINE_side_back();

// エンジェルラインの角度を取得
int get_LINE_deg();

#endif