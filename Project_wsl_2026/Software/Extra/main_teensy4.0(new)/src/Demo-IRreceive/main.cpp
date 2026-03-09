#include <Arduino.h>
#include "IRXIAO.h"

void setup()
{
  Serial.begin(9600);
  IRXIAObegin();
}

void loop()
{
  IRXIAOget();
  Serial.println("IRedge: " + String(IRedge) + " IRdist: " + String(IRdist));
  delay(100);
}