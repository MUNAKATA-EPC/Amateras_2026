#include <Arduino.h>
#include "bno.hpp"
#include "button.hpp"

bool is_ok = false;

void setup()
{
  resetButton.init(1, INPUT_PULLDOWN); // ボタン設置
  is_ok = bnoInit(&Wire, 0x28);        // bno初期化

  pinMode(A0, OUTPUT);
  analogWriteResolution(10); // 10bitで出力
}

void loop()
{
  resetButton.update();
  bnoUpdate(resetButton.isPushing());

  uint32_t output = (uint32_t)(bnoDeg() + 360 + 180) % 360;
  output = (uint32_t)roundf(map(output, 0, 360, 400, 500)); // なぜか400~560の範囲でしか出せない
  analogWrite(A0, output);

  Serial.println(output);

  delay(10);
}