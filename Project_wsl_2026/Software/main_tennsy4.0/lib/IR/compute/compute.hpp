#ifndef IR_COMPUTE_HPP
#define IR_COMPUTE_HPP

#include <Arduino.h>
#include "IR.hpp"

// 回り込む角度を出力
int get_IR_mawarikomi_deg();
// IRボールに近寄る角度を出力
int get_IR_follow_deg(int follow_target_offset);

#endif