#ifndef DSR1202_HPP
#define DSR1202_HPP

/*DSR1202とシリアル通信し、DSR1202を制御する*/

#include <Arduino.h>

// シリアル定義・開始
void DSR1202_init(HardwareSerial *serial, int baudrate);

// すべてのモーターを停止させる
void DSR1202_break();
// それぞれのチャンネルを動かす
void DSR1202_move(int value_1ch, int value_2ch, int value_3ch, int value_4ch);

#endif