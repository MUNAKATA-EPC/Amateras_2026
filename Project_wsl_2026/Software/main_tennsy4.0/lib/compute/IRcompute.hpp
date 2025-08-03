#ifndef IRCOMPUTE
#define IRCOMPUTE

#include <Arduino.h>
#include "IRserial.hpp"

// 回り込む角度を出力
int get_IR_mawarikomi_deg();
// IRボールに近づく角度を出力
int get_IR_follow_deg(int follow_target_offset);

#endif