#ifndef LINECOMPUTE_HPP
#define LINECOMPUTE_HPP

/*得たラインセンサーの状況から角度などを算出する*/

#include <Arduino.h>
#include "LINEserial.hpp"

// エンジェルラインの角度を取得
int get_LINE_deg();

// 右サイドの状況を取得
bool get_LINE_side_right();
// 左サイドの状況を取得
bool get_LINE_side_left();
// 後サイドの状況を取得
bool get_LINE_side_back();

#endif