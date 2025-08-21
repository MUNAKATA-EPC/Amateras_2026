#ifndef IR_SERIAL_HPP
#define IR_SERIAL_HPP

#include <Arduino.h>
#include "IR.hpp"

// シリアル定義・開始
void IR_init(HardwareSerial *serial, uint32_t baudrate);

// 更新
void IR_update();

// ボールが存在するかどうか
bool is_IR_exist();

// ボールの方向を出力
int16_t get_IR_deg();

// ボールまでの距離を出力
int16_t get_IR_distance();

#endif

