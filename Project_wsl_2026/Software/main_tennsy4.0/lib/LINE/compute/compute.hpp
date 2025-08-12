#ifndef LINECOMPUTE_HPP
#define LINECOMPUTE_HPP

#include <Arduino.h>
#include "LINE.hpp"

// === 更新 ===
// ノーマル版＆メモリ版の計算を同時に行う
void LINE_compute_update();

// === サイドの状況を取得（共通） ===
bool get_LINE_side_right(); // 右サイドの状況を取得
bool get_LINE_side_left();  // 左サイドの状況を取得
bool get_LINE_side_back();  // 後サイドの状況を取得

// === 角度取得 ===
// ノーマル版の角度を取得
int get_LINE_deg();
// メモリ版の角度を取得
int get_LINE_memory_deg();

// === 最初に反応したピン（共通） ===
int get_first_triggered_pin(); // 最初に反応したピンを取得

// === ハーフアウト検知（共通） ===
bool is_LINE_half_out(); // ハーフアウトしたかどうか

#endif
