#ifndef IR_COMPUTE_HPP
#define IR_COMPUTE_HPP

#include <Arduino.h>
#include "IR.hpp"

// 回り込む角度を出力
int16_t get_IR_mawarikomi_deg();

// IRボールに近寄る角度を出力
int16_t get_IR_follow_deg(int16_t follow_target_offset);

// IRボールの角度に比例する角度を出力
int16_t get_IR_hirei_deg(double a);

#endif
