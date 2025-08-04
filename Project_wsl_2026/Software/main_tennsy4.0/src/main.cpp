#include <Arduino.h>
// 攻撃用
#include "attacker.hpp"
// 守備用
#include "defender.hpp"
// テスト用
#include "PCprint.hpp"
// LCD用
#include "ui.hpp"

void setup()
{
  Serial.begin(9600); // PCとのシリアル通信を開始

  IRserial_init(&Serial1, 9600); // シリアル1を使いボートレート9600にする

  LINEserial_init(&Serial2, 9600); // シリアル2を使いボートレート9600にする

  DSR1202_init(&Serial3, 115200); // シリアル3を使いボートレート115200にする
  motors_init(45, 135, 225, 315); // それぞれのモータの角度を定義

  OpenMVserial_init(&Serial4, 115200); // シリアル4を使いボートレート115200にする

  BNO055_set_resetpin(22, INPUT_PULLDOWN); // BNOのリセットピンを定義
  BNO055_init(&Wire1, 0x28);               // どのBNOを使うか

  // catchsensor_init(1);     // キャッチセンサーのピンを設定
  kicker_set_fetpin(2, 3); // キッカーのFETピンを設定
  kicker_init(1000);       // クールダウン時間の定義

  ui_set_lcdpin(4, INPUT_PULLUP, 5, INPUT_PULLUP, 6, INPUT_PULLUP); // LCD用のボタンの定義
  ui_init();                                                        // 定義
}

void loop()
{

  IRserial_update();     // 更新
  LINEserial_update();   // 更新
  OpenMVserial_update(); // 更新
  BNO055_update();       // 更新

  // play_PCprint(TEST_ALL_CHECK_WITH_SERIAL); // 全て確認する
  play_attacker(); // 攻撃を開始
}