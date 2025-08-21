#ifndef DSR1202_HPP
#define DSR1202_HPP

#include <Arduino.h>
#include "Motors.hpp"

// モータ用のトグルスイッチのピン番号を設定
void DSR1202_set_motormove_togglepin(uint8_t pin);

// シリアル定義・開始
void DSR1202_init(HardwareSerial *serial, uint32_t baudrate);

// すべてのモーターを停止させる
void DSR1202_break();
// それぞれのチャンネルを動かす
void DSR1202_move(uint8_t value_1ch, uint8_t value_2ch, uint8_t value_3ch, uint8_t value_4ch);

#endif