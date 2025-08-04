#include <Arduino.h>
#include "multiplexer.hpp"

Multiplexer line_mux; // 定義

void setup()
{
  Serial.begin(9600);

  line_mux.set_pin(1, 2, 3, 4, 5, -1); // ピンを指定
  line_mux.init(10);                   // 時間を指定
}

void loop()
{
  for (int i = 0; i < 16; i++)
  {
    Serial.print("index");
    Serial.print(i);
    Serial.print(" : ");
    Serial.print(line_mux.read(i)); // 読み取る
    Serial.print(" , ");
  }
  Serial.println(); // 改行

  delay(10); // 10ms待機
}