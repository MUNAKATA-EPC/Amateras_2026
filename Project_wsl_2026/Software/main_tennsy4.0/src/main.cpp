#include <Arduino.h>
#include "IRserial.hpp"
#include "LINEserial.hpp"
#include "BNO055.hpp"

void setup()
{
  Serial.begin(9600); // PCとのシリアル通信を開始

  IRserial_init(&Serial1, 9600); // シリアル1を使いボートレート9600にする

  LINEserial_init(&Serial2, 9600); // シリアル2を使いボートレート9600にする

  BNO055_set_resetpin(22, INPUT_PULLDOWN); // BNOのリセットピンを定義
  BNO055_init(&Wire1, 0x28);               // どのBNOを使うか
}

void loop()
{
  IRserial_update(); // IRserialを更新
  Serial.print(get_IR_deg());
  Serial.print(" ");

  LINEserial_update(); // LINEserialを更新
  for (int i = 0; i < 19; i++)
  {
    Serial.print(get_LINE_data(i));
    Serial.print(" ");
  }

  BNO055_update(); // BNO055を更新
  Serial.print(BNO055_getDeg());
  Serial.println(); // 改行
}