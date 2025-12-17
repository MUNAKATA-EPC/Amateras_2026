#include <Arduino.h>
#include "bno.hpp"
#include "button.hpp"

bool is_ok = false;

void setup()
{
  resetButton.init(1, INPUT_PULLDOWN); // ボタン設置
  is_ok = bnoInit(&Wire, 0x28);        // bno初期化

  pinMode(0, OUTPUT);
  analogWriteResolution(10); // 10bitで出力
}

void loop()
{
  resetButton.update();
  bnoUpdate(resetButton.isPushing());

  Serial.println(String(is_ok) + " " + String(resetButton.isPushing()) + " " + String(bnoDeg()));

  int output = (int)roundf((bnoDeg() + 360) % 360 * 1023.0f / 360.0f);
  analogWrite(0, output);

  delay(10);
}