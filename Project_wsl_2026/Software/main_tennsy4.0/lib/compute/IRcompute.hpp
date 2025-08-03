#ifndef IRCOMPUTE
#define IRCOMPUTE

#include <Arduino.h>
#include "IRserial.hpp"

// 回り込む角度を取得
int get_IR_mawarikomi_deg(int deg);
// IRボールに近づく角度を取得
int get_IR_follow_deg(int deg, int distance, int follow_target_offset);

#endif