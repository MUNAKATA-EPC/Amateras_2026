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
const int LINEsensor_pin[LINE_SENSOR_COUNT] = {8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7}; // 前から左回りにピン番号を指定

unsigned int LINEsensor_adjust_value[LINE_SENSOR_COUNT] = {141, 155, 155, 138, 174, 160, 138, 136, 156, 137, 160, 163, 151, 150, 136, 108}; // センサー調整用値格納用

const int button_pin = 0;     // ボタンのピン番号
const int led_pin = 7;        // LEDのピン番号
const int max_led_power = 64; // LEDの最大出力

Button button;
void setup()
{
  Serial.begin(9600);
  Serial1.begin(115200);

  line_mux.set_pin(1, 2, 3, 4, 5, -1); // ピンを指定
  line_mux.init(10);                   // 時間を指定

  button.init(button_pin, INPUT_PULLDOWN); // ボタン初期化
  pinMode(led_pin, OUTPUT);                // LEDピン初期化

  // 一定時間以内にボタンを押すとセンサーの状況を送信するようになる
  bool adjust_flag = false;
  int brightness = 0;
  Timer led_timer;
  led_timer.reset();

  while (led_timer.msTime() < 5000UL) // 5秒待機
  {
    button.update();
    if (button.isReleased()) // ボタンが押されたら
    {
      adjust_flag = true;
      break;
    }

    brightness = (int)round(double(max_led_power / 2) * sin(double(led_timer.msTime()) * 2.0 * PI / 600.0) + double(max_led_power / 2)); // 600ms周期
    brightness = constrain(brightness, 0, max_led_power);

    analogWrite(led_pin, brightness); // LEDを滑らかに点滅
  }

  for (int i = brightness; i >= 0; i--)
  {
    analogWrite(led_pin, min(i, 0)); // LEDを滑らかに消灯
    delay(50);
  }

  // センサーの状況を調整用にTeensyに送信する
  Timer send_timer;
  send_timer.reset();
  led_timer.reset();

  if (adjust_flag)
  {
    do
    {
      button.update();

      // アナログでセンサーを読み取る
      int LINEsensor_values[LINE_SENSOR_COUNT];
      for (uint8_t i = 0; i < LINE_SENSOR_COUNT; i++)
      {
        LINEsensor_values[i] = line_mux.read(LINEsensor_pin[i]);
      }

      // 送信　ラインのアナログ値を送信
      if (send_timer.msTime() >= 10UL)
      {
        Serial1.write(start_header_to_adjust); // teensyとの通信開始
        for (uint8_t i = 0; i < LINE_SENSOR_COUNT; i++)
        {
          Serial1.write((uint8_t)(LINEsensor_values[i] & 0xFF));        // 2byteのデータなので下位の1byteのみ送信
          Serial1.write((uint8_t)((LINEsensor_values[i] >> 8) & 0xFF)); // 2byteのデータなので上位の1byteを送信
        }
        Serial1.write(end_header_to_adjust); // teensyとの通信終了

        send_timer.reset();
      }

      // LED点滅処理　遅
      if (led_timer.msTime() <= 1000UL)
        analogWrite(led_pin, max_led_power); // LED点灯
      else if (led_timer.msTime() <= 2000UL)
        analogWrite(led_pin, 0); // LED消灯
      else
        led_timer.reset();
    } while (!button.isReleased());

    analogWrite(led_pin, max_led_power); // LED点灯
    delay(5000);                         // 5秒待機
    analogWrite(led_pin, 0);             // LED消灯

    // 緑上にいるときの平均・最大値をとる
    const int ave_count = 60; // 60回の平均をとる

    unsigned int ring_court_sum[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned int ring_court_max[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    for (int i = 0; i < ave_count; i++)
    {
      // リング
      for (int mux_i = 0; mux_i < 16; mux_i++)
      {
        unsigned int ring_val = line_mux.read(LINEsensor_pin[mux_i]);
        ring_court_sum[mux_i] += ring_val;

        if (ring_val > ring_court_max[mux_i])
          ring_court_max[mux_i] = ring_val;
      }

      // LED点滅処理 速
      if (led_timer.msTime() <= 100UL)
        analogWrite(led_pin, max_led_power); // LED点灯
      else if (led_timer.msTime() <= 200UL)
        analogWrite(led_pin, 0); // LED消灯
      else
        led_timer.reset();

      delay(100);
    }

    // リングの平均値を出す
    for (int i = 0; i < 16; i++)
    {
      double ave = ring_court_sum[i] / ave_count;

      LINEsensor_adjust_value[i] = ring_court_max[i] + (ring_court_max[i] - (int)round(ave));
    }

    analogWrite(led_pin, max_led_power); // LED点灯
    delay(1000);                         // 1秒待機

    do
    {
      button.update();

      // 送信　調整した値を送信
      if (send_timer.msTime() >= 10UL)
      {
        Serial1.write(start_header_to_adjust); // teensyとの通信開始
        for (uint8_t i = 0; i < LINE_SENSOR_COUNT; i++)
        {
          Serial1.write((uint8_t)(LINEsensor_adjust_value[i] & 0xFF));        // 2byteのデータなので下位の1byteのみ送信
          Serial1.write((uint8_t)((LINEsensor_adjust_value[i] >> 8) & 0xFF)); // 2byteのデータなので上位の1byteを送信
        }
        Serial1.write(end_header_to_adjust); // teensyとの通信終了

        send_timer.reset();
      }

      // LED点滅処理　遅
      if (led_timer.msTime() <= 1000UL)
        analogWrite(led_pin, max_led_power); // LED点灯
      else if (led_timer.msTime() <= 2000UL)
        analogWrite(led_pin, 0); // LED消灯
      else
        led_timer.reset();
    } while (!button.isReleased());

    analogWrite(led_pin, 0); // LED消灯

    delay(1000); // 1秒待機
  }
}

void loop()
{
  // エンジェルライン
  uint32_t lines_data_bit_mask = 0UL; // 初期化

  for (uint8_t i = 0; i < 16; i++)
  {
    if (line_mux.read(LINEsensor_pin[i]) > LINEsensor_adjust_value[i]) // ラインが見えたら
      lines_data_bit_mask |= (1UL << i);
  }

  // 送信
  Serial1.write(start_header);                                  // teensyとの通信開始
  Serial1.write((uint8_t)(lines_data_bit_mask & 0xFF));         // 3byteのデータなので下位の1byteのみ送信
  Serial1.write((uint8_t)((lines_data_bit_mask >> 8) & 0xFF));  // 3byteのデータなので中位の1byteのみ送信
  Serial1.write((uint8_t)((lines_data_bit_mask >> 16) & 0xFF)); // 3byteのデータなので上位の1byteを送信
  Serial1.write(end_header);                                    // teensyとの通信終了

  Serial.println(lines_data_bit_mask, BIN); // pcに送る

  delay(10); // 10ms待機
}