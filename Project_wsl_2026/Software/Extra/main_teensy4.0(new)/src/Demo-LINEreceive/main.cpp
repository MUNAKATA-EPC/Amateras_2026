#include <Arduino.h>
#include "LINEXIAO.h"

void setup()
{
  Serial.begin(9600);
  LINEXIAObegin();
}

void loop()
{
  LINEXIAOget();
  Serial.println("LINEedge: " + String(LINEedge) + " LINEside: " + String(LINEright) + String(LINEleft) + String(LINEback));
  delay(100);
}