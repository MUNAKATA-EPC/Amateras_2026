#include <Arduino.h>
// IRについて
#include "IRserial.hpp"
#include "IRcompute.hpp"
// LINEについて
#include "LINEserial.hpp"
#include "LINEcompute.hpp"
// BNO055について
#include "BNO055.hpp"
// OpenMVについて
#include "OpenMV.hpp"
// DSR1202について
#include "DSR1202.hpp"
#include "motors.hpp"

void setup()
{
  Serial.begin(9600); // PCとのシリアル通信を開始

  IRserial_init(&Serial1, 9600); // シリアル1を使いボートレート9600にする

  LINEserial_init(&Serial2, 9600); // シリアル2を使いボートレート9600にする

  DSR1202_init(&Serial3, 115200); // シリアル3を使いボートレート115200にする
  motors_init(45, 135, 225, 315); // それぞれのモータの角度を定義

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