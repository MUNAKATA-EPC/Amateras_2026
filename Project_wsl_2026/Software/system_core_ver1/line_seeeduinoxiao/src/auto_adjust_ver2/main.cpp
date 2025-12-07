#include <Arduino.h>
#include "movement_average.hpp"
#include "multiplexer.hpp"
#include "button.hpp"

const int start_header = 0x55; // 同期ヘッダー格納用
const int end_header = 0xAA;   // 同期ヘッダー格納用

#define LINE_SENSOR_COUNT 16

Multiplexer line_mux;

const int LINEsensor_pin[LINE_SENSOR_COUNT] = {0, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1}; // 前から左回りにピン番号を指定
float LINEsensor_x[LINE_SENSOR_COUNT];                                                                // 前をx正方向
float LINEsensor_y[LINE_SENSOR_COUNT];                                                                // 前をx正方向

const int button_pin = 0; // ボタンのピン番号
const int led_pin = 7;    // LEDのピン番号

Button button;
Timer led_timer;

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

  button.init(button_pin, INPUT_PULLUP); // ボタン初期化
  pinMode(led_pin, OUTPUT);              // LEDピン初期化

  // 一定時間以内にボタンを押すとセンサーの状況を送信するようになる
  const float led_bright_hz = 2.0f; // 1s間に二周する
  bool adjust_flag = false;

  led_timer.reset();
  while (led_timer.msTime() < 3000UL) // 3秒待機
  {
    button.update();

    if (button.isReleased()) // ボタンが押されたら
    {
      adjust_flag = true;
      break;
    }

    digitalWrite(led_pin, HIGH); // LED ON
  }
}

void loop()
{
}