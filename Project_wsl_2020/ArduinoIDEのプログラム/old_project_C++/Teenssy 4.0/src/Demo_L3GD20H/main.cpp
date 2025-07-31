#include <Arduino.h>

void setup()
{
  Serial.begin(9600);     // USBシリアル（PC表示用）
  Serial5.begin(9600);    // D20 = TX7, D21 = RX7
}

void loop()
{
  if (Serial5.available())
  {
    String Seeed_Data = Serial5.readStringUntil('\n');
    Serial.println(Seeed_Data);
  }
}
