#include <Arduino.h>
#include "IRxiao.hpp"
#include "LINExiao.hpp"

IRxiao myIRball(&Serial2); // Serial2でIRのデータを取得
LINExiao myLine(&Serial3); // Serial3でLINEのデータ取得

void setup()
{
  Serial.begin(9600);
  myIRball.begin(9600);
  myLine.begin(9600);
}

void loop()
{
  myIRball.update();

  Serial.print(myIRball.isExist());
  Serial.print(" ");
  Serial.print(myIRball.getDeg());
  Serial.print(" ");
  Serial.print(myIRball.getDistance());
  Serial.print(" ");

  myLine.update();

  Serial.print(myLine.isExist());
  Serial.print(" ");
  Serial.print(myLine.getDeg());
  Serial.print(" ");
  Serial.print(myLine.getSideRight());
  Serial.print(myLine.getSideLeft());
  Serial.print(myLine.getSideBack());
  Serial.println();

  delay(100);
}