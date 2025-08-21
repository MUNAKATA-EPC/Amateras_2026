#include <Arduino.h>
#include "multiplexer.hpp"

/*黒ロボット用*/

const uint8_t head_byte = 0xAA; // 同期ヘッダー格納用

#define LINE_SIDE_RIGHT_PIN 8 // 右サイド
#define LINE_SIDE_LEFT_PIN 9  // 左サイド
#define LINE_SIDE_BACK_PIN 7  // 後サイド

#define LINE_ANGEL_JUDGE_VALUE 70 // エンジェルライン判定用の値
#define LINE_SIDE_JUDGE_VALUE 790 // サイドライン判定用の値

Multiplexer line_mux; // 定義

uint32_t lines_data_bit_mask; // 16+3個のラインセンサーの状況格納用

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
  /*エンジェルラインについて*/
  for (uint8_t i = 0; i < 16; i++)
  {
    if (line_mux.read(i) > LINE_ANGEL_JUDGE_VALUE) // ラインが見えたら
      lines_data_bit_mask |= (1 << i);
  }

  /*サイドラインについて*/
  if (analogRead(LINE_SIDE_RIGHT_PIN) > LINE_SIDE_JUDGE_VALUE && analogRead(LINE_SIDE_RIGHT_PIN) < 1020) // 右サイドのラインセンサーがラインを見ているか
    lines_data_bit_mask |= (1 << 16);

  if (analogRead(LINE_SIDE_LEFT_PIN) > LINE_SIDE_JUDGE_VALUE && analogRead(LINE_SIDE_LEFT_PIN) < 1020) // 右サイドのラインセンサーがラインを見ているか
    lines_data_bit_mask |= (1 << 17);

  if (analogRead(LINE_SIDE_BACK_PIN) > LINE_SIDE_JUDGE_VALUE && analogRead(LINE_SIDE_BACK_PIN) < 1020) // 右サイドのラインセンサーがラインを見ているか
    lines_data_bit_mask |= (1 << 18);

  /*送信*/
  Serial1.write(head_byte);                 // teensyとの通信開始
  Serial1.write(lines_data_bit_mask);       // 3byteのデータなので下位の1byteのみ送信
  Serial1.write(lines_data_bit_mask >> 8);  // 3byteのデータなので中位の1byteのみ送信
  Serial1.write(lines_data_bit_mask >> 16); // 3byteのデータなので上位の1byteを送信

  Serial1.flush(); // 送信バッファがなくなるまで、つまり全て送信するまで待つ

  Serial1.println(lines_data_bit_mask, BIN); // pcに送る
  Serial.print("l");
  Serial.print(analogRead(LINE_SIDE_LEFT_PIN));
  Serial.print("r");
  Serial.print(analogRead(LINE_SIDE_RIGHT_PIN));
  Serial.print("b");
  Serial.println(analogRead(LINE_SIDE_BACK_PIN));

  delay(10); // 10ms待機
}