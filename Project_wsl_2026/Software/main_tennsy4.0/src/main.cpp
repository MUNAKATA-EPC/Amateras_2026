#include <Arduino.h>
#include "IRxiao.hpp"
#include "LINExiao.hpp"
#include "BNO055.hpp"

IRxiao myIRball(&Serial2); // Serial2でIRのデータを取得
LINExiao myLine(&Serial3); // Serial3でLINEのデータ取得

Adafruit_BNO055 adafBno(0xA0, 0x28, &Wire1);
BNO055 myBno(&adafBno);

void setup()
{
  Serial.begin(9600); // USBシリアルを開始

  myIRball.begin(9600); // IRxiaoとのシリアル開始
  myLine.begin(9600);   // LINExiaoとのシリアル開始

  myBno.setresetPin(22, INPUT_PULLUP); // BNO055のリセットピン設定
  myBno.begin();                       // BNO055の通信確立まで待つ
}

void loop()
{
  myIRball.update(); // 更新

  Serial.print(myIRball.isExist());
  Serial.print(" ");
  Serial.print(myIRball.getDeg());
  Serial.print(" ");
  Serial.print(myIRball.getDistance());
  Serial.print(" ");

  myLine.update(); // 更新

  Serial.print(myLine.isExist());
  Serial.print(" ");
  for (int i = 0; i < 19; i++)
  {
    Serial.print(myLine.getData(i));
    Serial.print(" ");
  }

  myBno.update(); // 更新

  Serial.print(myBno.getDeg());

  Serial.println(); // 改行

  delay(100);
}