#include <Arduino.h>
#include "multiplexer.hpp"

/*黒ロボット用*/
#define LINE_SIDE_RIGHT_PIN 8 // 右サイド
#define LINE_SIDE_LEFT_PIN 9  // 左サイド
#define LINE_SIDE_BACK_PIN 10 // 後サイド

#define LINE_ANGEL_JUDGE_VALUE 80 // エンジェルライン判定用の値
#define LINE_SIDE_JUDGE_VALUE 790 // サイドライン判定用の値

const uint8_t head_byte = 0xAA; // 同期ヘッダー格納用

uint32_t lines_data_bit_mask; // 16+3個のラインセンサーの状況格納用

Multiplexer line_mux; // 定義

void setup()
{
  Serial.begin(9600);
  Serial1.begin(115200);

  pinMode(LINE_SIDE_RIGHT_PIN, INPUT); // サイド用のピン
  pinMode(LINE_SIDE_LEFT_PIN, INPUT);  // サイド用のピン
  pinMode(LINE_SIDE_BACK_PIN, INPUT);  // サイド用のピン

  line_mux.set_pin(1, 2, 3, 4, 5, -1); // ピンを指定
  line_mux.init(10);                   // 時間を指定
}

void loop()
{
  lines_data_bit_mask = 0; // 初期化

  /*エンジェルラインについて*/
  for (uint8_t i = 0; i < 16; i++)
  {
    if (line_mux.read(i) > LINE_ANGEL_JUDGE_VALUE) // ラインが見えたら
      lines_data_bit_mask |= (1UL << i);
  }

  /*サイドラインについて*/
  uint32_t right_val = analogRead(LINE_SIDE_RIGHT_PIN); // 右サイドのラインセンサー
  uint32_t left_val = analogRead(LINE_SIDE_LEFT_PIN);   // 左サイドのラインセンサー
  uint32_t back_val = analogRead(LINE_SIDE_BACK_PIN);   // 後サイドのラインセンサー

  if (right_val > LINE_SIDE_JUDGE_VALUE && right_val < 1020) // 右サイドのラインセンサーがラインを見ているか
    lines_data_bit_mask |= (1UL << 16);

  if (left_val > LINE_SIDE_JUDGE_VALUE && left_val < 1020) // 右サイドのラインセンサーがラインを見ているか
    lines_data_bit_mask |= (1UL << 17);

  if (back_val > LINE_SIDE_JUDGE_VALUE && back_val < 1020) // 右サイドのラインセンサーがラインを見ているか
    lines_data_bit_mask |= (1UL << 18);

  /*送信*/
  Serial1.write(head_byte);                                     // teensyとの通信開始
  Serial1.write((uint8_t)(lines_data_bit_mask & 0xFF));         // 3byteのデータなので下位の1byteのみ送信
  Serial1.write((uint8_t)((lines_data_bit_mask >> 8) & 0xFF));  // 3byteのデータなので中位の1byteのみ送信
  Serial1.write((uint8_t)((lines_data_bit_mask >> 16) & 0xFF)); // 3byteのデータなので上位の1byteを送信

  /*Serial1.print((int)lines_data_bit_mask); // 文字列として送信
  Serial1.print("a");
  Serial1.flush(); // 送信バッファがなくなるまで、つまり全て送信するまで待つ*/

  /*Serial.print(lines_data_bit_mask, BIN); // pcに送る
  Serial.print("l");
  Serial.print(left_val);
  Serial.print("r");
  Serial.print(right_val);
  Serial.print("b");
  Serial.println(back_val);*/

  delay(10); // 10ms待機
}