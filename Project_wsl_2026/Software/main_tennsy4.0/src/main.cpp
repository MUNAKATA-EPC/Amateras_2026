#include <Arduino.h>
// 攻撃用
#include "attacker.hpp"
// 守備用
#include "defender.hpp"
// テスト用
#include "test.hpp"

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
  play_test(TEST_ALL_CHECK_WITH_SERIAL); // 全て確認する
}