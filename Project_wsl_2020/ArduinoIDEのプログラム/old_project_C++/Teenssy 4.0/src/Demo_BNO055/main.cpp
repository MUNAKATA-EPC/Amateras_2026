#include <Arduino.h>
// BNO関連のライブラリー
#include "USE_BNO.h"

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(22, INPUT_PULLDOWN);
  BNObegin(22);
}

void loop()
{
  Serial.println(BNOread());
  delay(100);
}