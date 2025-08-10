#include <Arduino.h>
#include "multiplexer.hpp"

/*白ロボット用*/

#define LINE_SIDE_RIGHT_PIN 8 // 右サイド
#define LINE_SIDE_LEFT_PIN 9  // 左サイド
#define LINE_SIDE_BACK_PIN 10 // 後サイド

#define LINE_ANGEL_JUDGE_VALUE 500 // エンジェルライン判定用の値
#define LINE_SIDE_JUDGE_VALUE 700  // サイドライン判定用の値

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
  /*エンジェルラインについて*/
  unsigned int line_data_10 = 0; // 変換した10進数のデータ格納用
  for (int i = 0; i < 16; i++)
  {
    if (line_mux.read(i) > LINE_ANGEL_JUDGE_VALUE) // ラインが見えたら
      line_data_10 += pow(2, i);
  }

  /*サイドラインについて*/
  if (analogRead(LINE_SIDE_RIGHT_PIN) > LINE_SIDE_JUDGE_VALUE) // 右サイドのラインセンサーがラインを見ているか
    line_data_10 += pow(2, 16);                                // 右サイドのラインセンサーがラインを見ている場合は16ビット目に1をセット

  if (analogRead(LINE_SIDE_LEFT_PIN) > LINE_SIDE_JUDGE_VALUE) // 右サイドのラインセンサーがラインを見ているか
    line_data_10 += pow(2, 17);                               // 右サイドのラインセンサーがラインを見ている場合は17ビット目に1をセット

  if (analogRead(LINE_SIDE_BACK_PIN) > LINE_SIDE_JUDGE_VALUE) // 右サイドのラインセンサーがラインを見ているか
    line_data_10 += pow(2, 18);                               // 右サイドのラインセンサーがラインを見ている場合は18ビット目に1をセット

  /*送信*/
  Serial1.println(line_data_10); // teensyに送る
  Serial1.flush();

  // Serial.println(line_data_10);  // pcに送る
  Serial.print("l");
  Serial.print(analogRead(LINE_SIDE_LEFT_PIN));
  Serial.print("r");
  Serial.print(analogRead(LINE_SIDE_RIGHT_PIN));
  Serial.print("b");
  Serial.println(analogRead(LINE_SIDE_BACK_PIN));

  delay(10); // 10ms待機
}