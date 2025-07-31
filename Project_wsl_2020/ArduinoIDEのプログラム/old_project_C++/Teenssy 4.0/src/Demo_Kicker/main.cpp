#include <Arduino.h>

void setup()
{
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(22, INPUT_PULLDOWN);
}

void loop()
{
  if (digitalRead(22) == HIGH)
  {
    digitalWrite(2, LOW);
    digitalWrite(3, HIGH);

    digitalWrite(4, HIGH);
  }
  else
  {
    digitalWrite(2, HIGH);
    digitalWrite(3, LOW);

    digitalWrite(4, LOW);
  }
}