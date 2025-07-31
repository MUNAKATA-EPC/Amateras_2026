#include <Arduino.h>
#include "USE_LCD.h"
#include "USE_m5pico.h"
#include "USE_DSR1202.h"
#include "USE_Kicker.h"
#include "USE_BNO.h"
#include "USE_L3GD20H.h"
#include "USE_PIDv2.h"
#include "USE_ControlSW.h"
#include "USE_NeoPixel.h"

// モータの最大出力値
const int CONTROLMAXPOWER = 24;
const int GYROMAXPOWER = 16;

// LCDの表示時間の設定
const int LCD_ShowLogo_ms = 0;
const int m5pico_TalkStart_ms = 4500;

void setup()
{
  // PCとのシリアルの設定
  Serial.begin(115200);

  // M5-picoとのシリアルの設定
  M5PICObegin(9600);    // D6をRX,D7をTXとしている。通信速度は9600でM5-picoと通信開始
  Stick_Adjust(10, 10); // AnalogStickの-1とする範囲の調整。X・Yどちらも±5は-1としている。

  // DSRとのシリアルの設定
  DSR1202begin(1);
  DSR1202motorGain(1, 1.23, 0.9, 1.1);
  DSR1202move(0, 0, 0, 0); // モータは停止させる

  // L3GD20Hとのシリアル設定
  L3GD20Hbegin(9600);

  // BNOとのI2C通信の設定
  pinMode(22, INPUT_PULLDOWN);
  BNObegin(22);

  // LCDとのI2C通信の設定
  LCDbegin(0x3C); // アドレスを0x3Cとして通信開始

  // PIDを開始する
  PIDv2begin(0.18, 0, 0.01, GYROMAXPOWER, 6);

  // コントロール基板上のタクトスイッチのピンを定義
  pinMode(10, INPUT_PULLDOWN); // 左のタクトスイッチ
  pinMode(11, INPUT_PULLDOWN); // 真ん中のタクトスイッチ
  pinMode(12, INPUT_PULLDOWN); // 右のタクトスイッチ
  ControlSWbegin(12, 11, 10);  // どのピンを使うか定義

  // キッカーの設定
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  Kickerbegin(2, 3);

  // NeoPixelの設定
  NeoPixel_begin(24, 23, 15);
}

void loop()
{
  // ユーザーが選んだプログラムを開始させる
  int SelectNum = SelectMenu();

  switch (SelectNum)
  {
  case 0:
    // PlaySoccer_App1起動
    App1_PlaySoccer();
    break;
  case 1:
    // PlaySoccer_App2起動
    App2_PlaySoccer();
    break;
  case 2:
    // PIDChecker_App2起動
    App2_PIDChecker();
    break;
  case 3:
    // PIDChecker_App2起動
    App2_PIDChecker();
    break;
  case 4:
    // PS3Checker_App1を起動させる
    App1_PS3Checker();
    break;
  case 5:
    // PS3Checker_App2を起動させる
    App2_PS3Checker();
    break;
  }

  // モータは停止させる
  DSR1202move(0, 0, 0, 0);
}