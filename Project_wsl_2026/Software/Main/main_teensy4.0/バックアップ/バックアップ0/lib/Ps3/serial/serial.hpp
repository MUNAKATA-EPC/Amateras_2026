#ifndef PS3_SERIAL_HPP
#define PS3_SERIAL_HPP

#include <Arduino.h>
#include "Ps3.hpp"

// 調整値定義
void Ps3_set_stick_adjust(int8_t lx_adjust, int8_t ly_adjust, int8_t rx_adjust, int8_t ry_adjust);
// シリアル定義・開始
void Ps3_init(HardwareSerial *serial, uint32_t baudrate);
// 更新
void Ps3_serial_update();

// 左スティックが触られたかどうか
bool is_Ps3_stick_left_move();
// 左ステックのX方向を出力
int get_Ps3_stick_lx();
// 左ステックのY方向を出力
int get_Ps3_stick_ly();

// 右スティックが触られたかどうか
bool is_Ps3_stick_right_move();
// 右ステックのX方向を出力
int get_Ps3_stick_rx();
// 右ステックのY方向を出力
int get_Ps3_stick_ry();

// buttonのデータを出力
bool get_Ps3_button_data(uint8_t index);

#endif
