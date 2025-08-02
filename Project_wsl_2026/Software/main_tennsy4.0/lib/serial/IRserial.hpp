#ifndef IRSERIAL_HPP
#define IRSERIAL_HPP

/*IRボールの状況をシリアル通信で受信する*/

#include <Arduino.h>

// シリアル定義・開始
void IRserial_init(HardwareSerial *serial, int baudrate);
// 更新
void IRserial_update();

// ボールが存在するかどうか
bool is_IR_exist();
// ボールの方向を出力
int get_IR_deg();
// ボールまでの距離を出力
int get_IR_distance();

#endif
