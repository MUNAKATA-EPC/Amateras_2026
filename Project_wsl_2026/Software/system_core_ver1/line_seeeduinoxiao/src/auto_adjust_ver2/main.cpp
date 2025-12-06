#include <Arduino.h>
#include "movement_average.hpp"
#include "multiplexer.hpp"

const int head_byte = 0xAA; // 同期ヘッダー格納用

#define LINE_SENSOR_COUNT 16

Multiplexer line_mux;

const int LINEsensor_pin[LINE_SENSOR_COUNT] = {0, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1}; // 前から左回りにピン番号を指定

float LINEsensor_x[LINE_SENSOR_COUNT]; // 前をx正方向
float LINEsensor_y[LINE_SENSOR_COUNT]; // 前をx正方向

void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600);

  line_mux.set_pin(1, 2, 3, 4, 5, -1); // ピンを指定
  line_mux.init(10);                   // 時間を指定

  float diff_angle = 360.0f / LINE_SENSOR_COUNT;

  for (int i = 0; i < LINE_SENSOR_COUNT; i++)
  {
    LINEsensor_x[i] = cosf(radians(i * diff_angle)); // 座標の計算
    LINEsensor_y[i] = sinf(radians(i * diff_angle)); // 座標の計算
  }
}

void loop()
{
}