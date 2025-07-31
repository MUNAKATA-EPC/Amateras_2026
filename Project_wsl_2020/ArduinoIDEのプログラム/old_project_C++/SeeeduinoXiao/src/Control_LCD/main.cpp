#include <Arduino.h>
#include "USE_m5pico.h"
#include "USE_LCD.h"

const int LCD_ShowLogo_ms = 3000;
const int m5pico_TalkStart_ms = 4500;

void setup()
{
  Serial.begin(115200);
  M5PICObegin(9600);  // D6をRX,D7をTXとしている。通信速度は9600でM5-picoと通信開始
  Stick_Adjust(8, 8); // AnalogStickの-1とする範囲の調整。X・Yどちらも±5は-1としている。
  LCDbegin(0x3C);     // アドレスを0x3Cとして通信開始

  App1_PS3Checker(); // コントローラーとの接続確認のためにPS3Checkerを起動
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
    // PIDChecker_App1起動
    App1_PIDChecker();
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
  while (SW_cross)
  {
    M5PICOget();
    delay(100);
  }
}