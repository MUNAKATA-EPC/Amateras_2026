#include <Arduino.h>
#include "multiplexer.hpp"

/*seidaiロボット用*/
const uint8_t head_byte = 0xAA; // 同期ヘッダー格納用
const uint8_t LINE_SIDE_PINS[3] = {8, 9, 10}; // 右・左・後サイドのピン

uint32_t lines_data_bit_mask; // 16+3個のラインセンサーの状況格納用

Multiplexer line_mux; // 定義

// 自動調整する
unsigned int LINE_RING_JUDGE_VALUE[16] = {80}; // エンジェルライン判定用の値
unsigned int LINE_SIDE_JUDGE_VALUE[3] = {790}; // 右・左・後サイドライン判定用の値

void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600);

  for (int i = 0; i < 3; i++)
  {
    pinMode(LINE_SIDE_PINS[i], INPUT); // サイド用のピン
  }

  line_mux.set_pin(1, 2, 3, 4, 5, -1); // ピンを指定
  line_mux.init(10);                   // 時間を指定

  // 緑上にいるときの平均・最大値をとる
  const int ave_count = 60; // 60回の平均をとる

  unsigned int ring_court_sum[16] = {0};
  unsigned int ring_court_max[16] = {0};

  unsigned int side_court_sum[3] = {0};
  unsigned int side_court_max[3] = {0};
  for (int i = 0; i < ave_count; i++)
  {
    // リング
    for (int mux_i = 0; mux_i < 16; mux_i++)
    {
      unsigned int ring_val = line_mux.read(mux_i);
      ring_court_sum[mux_i] += ring_val;

      if (ring_val > ring_court_max[mux_i])
        ring_court_max[mux_i] = ring_val;
    }

    // サイド
    for (int i = 0; i < 3; i++)
    {
      unsigned int side_val = analogRead(LINE_SIDE_PINS[i]);
      side_court_sum[i] += side_val;

      if (side_val > side_court_max[i])
        side_court_max[i] = side_val;
    }

    delay(50);
  }

  // リングの平均値を出す
  for (int i = 0; i < 16; i++)
  {
    double ave = ring_court_sum[i] / ave_count;
    // LINE_RING_JUDGE_VALUE[i] = (int)round(ave);
    LINE_RING_JUDGE_VALUE[i] = ring_court_max[i] + (ring_court_max[i] - (int)round(ave));
  }

  // サイドの平均値を出す
  for (int i = 0; i < 3; i++)
  {
    double ave = side_court_sum[i] / ave_count;
    // LINE_SIDE_JUDGE_VALUE[i] = (int)round(ave);
    LINE_SIDE_JUDGE_VALUE[i] = side_court_max[i] + (side_court_max[i] - (int)round(ave));
  }
}

void loop()
{
  lines_data_bit_mask = 0; // 初期化

  /*エンジェルラインについて*/
  for (uint8_t i = 0; i < 16; i++)
  {
    if (line_mux.read(i) > LINE_RING_JUDGE_VALUE[i]) // ラインが見えたら
      lines_data_bit_mask |= (1UL << i);
  }

  /*サイドラインについて*/
  uint32_t right_val = analogRead(LINE_SIDE_PINS[0]); // 右サイドのラインセンサー
  uint32_t left_val = analogRead(LINE_SIDE_PINS[1]);  // 左サイドのラインセンサー
  uint32_t back_val = analogRead(LINE_SIDE_PINS[2]);  // 後サイドのラインセンサー

  if (right_val > LINE_SIDE_JUDGE_VALUE[0] && right_val < 1020) // 右サイドのラインセンサーがラインを見ているか
    lines_data_bit_mask |= (1UL << 16);

  if (left_val > LINE_SIDE_JUDGE_VALUE[1] && left_val < 1020) // 右サイドのラインセンサーがラインを見ているか
    lines_data_bit_mask |= (1UL << 17);

  /*if (back_val > LINE_SIDE_JUDGE_VALUE[2] && back_val < 1020) // 右サイドのラインセンサーがラインを見ているか
    lines_data_bit_mask |= (1UL << 18);*/

  /*送信*/
  Serial1.write(head_byte);                                     // teensyとの通信開始
  Serial1.write((uint8_t)(lines_data_bit_mask & 0xFF));         // 3byteのデータなので下位の1byteのみ送信
  Serial1.write((uint8_t)((lines_data_bit_mask >> 8) & 0xFF));  // 3byteのデータなので中位の1byteのみ送信
  Serial1.write((uint8_t)((lines_data_bit_mask >> 16) & 0xFF)); // 3byteのデータなので上位の1byteを送信

  Serial.print(lines_data_bit_mask, BIN); // pcに送る
  Serial.print("l");
  Serial.print(left_val);
  Serial.print("r");
  Serial.print(right_val);
  Serial.print("b");
  Serial.println(back_val);

  delay(10); // 10ms待機
}