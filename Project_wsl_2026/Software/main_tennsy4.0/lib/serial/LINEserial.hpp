#ifndef LINESERIAL_HPP
#define LINESERIAL_HPP

#include <Arduino.h>

// シリアル定義・開始
void LINEserial_init(HardwareSerial *serial, int baudrate);
// 更新
void LINEserial_update();

// ラインが存在するかどうか
bool is_LINE_exist();
// LINEのデータを出力
int get_LINE_data(int pin);

#endif