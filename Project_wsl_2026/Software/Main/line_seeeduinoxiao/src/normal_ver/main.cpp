#include <Arduino.h>
#include "movement_average.hpp"
#include "multiplexer.hpp"
#include "button.hpp"

// 通常用の送信ヘッダー
const int start_header = 0x55; // 同期ヘッダー格納用
const int end_header = 0xAA;   // 同期ヘッダー格納用

Multiplexer line_mux;

#define LINE_SENSOR_COUNT 16
const int LINEsensor_pin[LINE_SENSOR_COUNT] = {8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7}; // 前から左回りにピン番号を指定

const int LINEsensor_side_right_pin = A9; // 右サイドのピン
const int LINEsensor_side_left_pin = A8;  // 左サイドのピン

unsigned int LINEsensor_adjust_value[LINE_SENSOR_COUNT] =
    {300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300}; // センサー調整用値格納用

unsigned int LINEsensor_side_right_adjust_value = 600U; // 右サイドの調整値
unsigned int LINEsensor_side_left_adjust_value = 600U;  // 左サイドの調整値

const int button_pin = 0; // ボタンのピン番号
const int led_pin = 7;    // LEDのピン番号

Button button;
void setup()
{
  Serial.begin(9600);
  Serial1.begin(115200);

  line_mux.set_pin(1, 2, 3, 4, 5, -1); // ピンを指定
  line_mux.init(10);                   // 時間を指定

  button.init(button_pin, INPUT_PULLDOWN); // ボタン初期化
  pinMode(led_pin, OUTPUT);                // LEDピン初期化
  digitalWrite(led_pin, LOW);

  pinMode(LINEsensor_side_right_pin, INPUT); // 右サイドのピン設定
  pinMode(LINEsensor_side_left_pin, INPUT);  // 左サイドのピン設定
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

  // サイドライン
  if (analogRead(LINEsensor_side_right_pin) >= LINEsensor_side_right_adjust_value)
  {
    lines_data_bit_mask |= (1UL << 16);
  }
  if (analogRead(LINEsensor_side_left_pin) >= LINEsensor_side_left_adjust_value)
  {
    lines_data_bit_mask |= (1UL << 17);
  }

  // 送信
  Serial1.write(start_header);                                  // teensyとの通信開始
  Serial1.write((uint8_t)(lines_data_bit_mask & 0xFF));         // 3byteのデータなので下位の1byteのみ送信
  Serial1.write((uint8_t)((lines_data_bit_mask >> 8) & 0xFF));  // 3byteのデータなので中位の1byteのみ送信
  Serial1.write((uint8_t)((lines_data_bit_mask >> 16) & 0xFF)); // 3byteのデータなので上位の1byteを送信
  Serial1.write(end_header);                                    // teensyとの通信終了
  // Serial.print(String(analogRead(LINEsensor_side_right_pin)) + " " + String(analogRead(LINEsensor_side_left_pin)) + " ");
  // Serial.println(lines_data_bit_mask, BIN); // pcに送る
  Serial1.flush();

  delay(10);
}