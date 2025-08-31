#include <Arduino.h>
#include "BNO055.h"

void setup()
{
  Serial.begin(9600);
  BNO055begin();
}

void loop()
{
  BNO055get();
  Serial.println(BNO055deg);
  delay(100);
}