#include <Arduino.h>
#include "USE_LCD.h"
#include "USE_m5pico.h"
#include "USE_DSR1202.h"
#include "USE_Kicker.h"
#include "USE_BNO.h"
#include "PID.h"

const int CONTROLMAXPOWER = 20;
const int GYROMAXPOWER = 15;

const int LCD_ShowLogo_ms = 0;
const int m5pico_TalkStart_ms = 4500;

void setup()
{
  // PCとのシリアル
  Serial.begin(115200);

  // BNOとの通信を開始させる
  pinMode(22, INPUT_PULLDOWN);
  BNObegin(22);

  // PIDの初期設定
  PIDbegin(0.15, 0.02, 0, GYROMAXPOWER, 3);

  // M5-picoの設定
  M5PICObegin(9600);  // D6をRX,D7をTXとしている。通信速度は9600でM5-picoと通信開始
  Stick_Adjust(8, 8); // AnalogStickの-1とする範囲の調整。X・Yどちらも±5は-1としている。

  // LCDの設定
  LCDbegin(0x3C); // アドレスを0x3Cとして通信開始

  // キッカーの設定
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  Kickerbegin(2, 3);

  // シリアル1でDSRと通信する
  DSR1202begin(1);
  DSR1202motorGain(1.1, 0.78, 1.35, 1.03);

  // コントロール基板上のタクトスイッチのピンを定義
  pinMode(10, INPUT_PULLDOWN); // 左のタクトスイッチ
  pinMode(11, INPUT_PULLDOWN); // 真ん中のタクトスイッチ
  pinMode(12, INPUT_PULLDOWN); // 右のタクトスイッチ

  App1_PlaySoccer(); // まずPlaySoccerを起動させる

  // モータは停止させる
  DSR1202move(0, 0, 0, 0);
}

void loop()
{
  // ユーザーが選んだプログラムを開始させる
  int SelectNum = SelectMenu();
  // Serial.println(SelectNum);
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

  while (SW_cross)
  {
    M5PICOget();
    delay(100);
  }
}