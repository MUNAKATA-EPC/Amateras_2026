#include <Arduino.h>
#include "SW.h"

void setup()
{
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  SWbegin();
}

void loop()
{
  TactSWget();
  if (EnterSW_true)
  {
    digitalWrite(2, HIGH);
    digitalWrite(3, LOW);
  }
  else
  {
    digitalWrite(2, LOW);
    digitalWrite(3, HIGH);
  }
  delay(100);
}