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
int get_IR_deg();

// ボールまでの距離を出力
int get_IR_distance();

// ボールの値を出力
int get_IR_value();

#endif
