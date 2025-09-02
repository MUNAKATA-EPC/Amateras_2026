#ifndef LINE_SERIAL_HPP
#define LINE_SERIAL_HPP

#include <Arduino.h>
#include "LINE.hpp"

// シリアル定義・開始
void LINE_init(HardwareSerial *serial, uint32_t baudrate);

// 更新
void LINE_serial_update();

// ラインが存在するかどうか
bool is_LINE_exist();

// LINEのデータを出力
bool get_LINE_data(uint8_t index);

#endif
