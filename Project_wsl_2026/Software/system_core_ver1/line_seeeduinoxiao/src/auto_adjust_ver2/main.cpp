#include <Arduino.h>
#include "movement_average.hpp"
#include "multiplexer.hpp"
#include "button.hpp"

// 調整用の送信ヘッダー
const int start_header_to_adjust = 0x66; // 同期ヘッダー格納用
const int end_header_to_adjust = 0xBB;   // 同期ヘッダー格納用
// 通常用の送信ヘッダー
const int start_header = 0x55; // 同期ヘッダー格納用
const int end_header = 0xAA;   // 同期ヘッダー格納用

Multiplexer line_mux;

#define LINE_SENSOR_COUNT 16
const int LINEsensor_pin[LINE_SENSOR_COUNT] = {0, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1}; // 前から左回りにピン番号を指定
float LINEsensor_x[LINE_SENSOR_COUNT];                                                                // 前をx正方向
float LINEsensor_y[LINE_SENSOR_COUNT];                                                                // 前をx正方向

unsigned int LINEsensor_adjust_value[LINE_SENSOR_COUNT] = {500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 500}; // センサー調整用値格納用

const int button_pin = 0; // ボタンのピン番号
const int led_pin = 7;    // LEDのピン番号

Button button;
Timer led_timer;

void setup()
{
  Serial.begin(9600);
  Serial1.begin(115200);

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

    int brightness = round(127.5 * sin(double(led_timer.msTime()) * 2.0 * PI / 500.0) + 127.5); // 500ms周期
    brightness = constrain(brightness, 0, 255);

    analogWrite(led_pin, brightness); // LEDを滑らかに点滅
  }

  // センサーの状況を調整用にTeensyに送信する
  led_timer.reset();

  while (adjust_flag)
  {
    button.update();
    if (button.isReleased()) // ボタンが押されたら
      break;

    // アナログでセンサーを読み取る
    int LINEsensor_values[LINE_SENSOR_COUNT];
    for (uint8_t i = 0; i < LINE_SENSOR_COUNT; i++)
    {
      LINEsensor_values[i] = line_mux.read(i);
    }

    // 送信
    Serial1.write(start_header_to_adjust); // teensyとの通信開始
    for (uint8_t i = 0; i < LINE_SENSOR_COUNT; i++)
    {
      Serial1.write((uint8_t)(LINEsensor_values[i] & 0xFF));        // 2byteのデータなので下位の1byteのみ送信
      Serial1.write((uint8_t)((LINEsensor_values[i] >> 8) & 0xFF)); // 2byteのデータなので上位の1byteを送信
    }
    Serial1.write(end_header_to_adjust); // teensyとの通信終了

    // LED点滅処理
    if (led_timer.msTime() <= 1000UL)
      analogWrite(led_pin, 255); // LED点灯
    else if (led_timer.msTime() <= 2000UL)
      analogWrite(led_pin, 0); // LED消灯
    else
      led_timer.reset();

    delay(10); // 10ms待機
  }
}

void loop()
{
  // エンジェルライン
  uint32_t lines_data_bit_mask = 0U; // 初期化

  for (uint8_t i = 0; i < 16; i++)
  {
    if (line_mux.read(i) > LINEsensor_adjust_value[i]) // ラインが見えたら
      lines_data_bit_mask |= (1U << i);
  }

  // 送信
  Serial1.write(start_header);                                  // teensyとの通信開始
  Serial1.write((uint8_t)(lines_data_bit_mask & 0xFF));         // 3byteのデータなので下位の1byteのみ送信
  Serial1.write((uint8_t)((lines_data_bit_mask >> 8) & 0xFF));  // 3byteのデータなので中位の1byteのみ送信
  Serial1.write((uint8_t)((lines_data_bit_mask >> 16) & 0xFF)); // 3byteのデータなので上位の1byteを送信
  Serial1.write(end_header);                                    // teensyとの通信終了

  delay(10); // 10ms待機
}