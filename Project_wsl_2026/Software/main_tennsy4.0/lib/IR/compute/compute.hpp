#ifndef IR_COMPUTE_HPP
#define IR_COMPUTE_HPP

#include <Arduino.h>
#include "IR.hpp"

// 回り込む角度を出力
int get_IR_mawarikomi_deg();

// ボールの接線角度を計算し、回り込みに最適な方を出力
int get_IR_sessen_deg(double R, double dist_adjust_value);

// IRボールに近寄る角度を出力
int get_IR_follow_deg(int follow_target_offset);

// IRボールの角度に比例する角度を出力
int get_IR_hirei_deg(double a);

//距離の逆数に比例した値を出力
int get_IR_hirei_distance(double a, int dist_adjust_value);

#endif
