/*フォトトランジスタを使う場合のプログラム*/
#include <Arduino.h>
#include "MyLib-Multi16ch.h"
#include "MySettings.h"

// エンジェルラインについて
const int LINEsensor_pin[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}; // 前から反時計回りに指定

int LINEsensor_value[16];                    // LINEセンサーの値を格納
int LINEsensor_condition_count;              // ラインセンサーが何個反応しているかカウント
bool LINEsensor_old_condition[16] = {false}; // 昔のラインセンサーは反応していたか
double LINEposition_of_x, LINEposition_of_y; // IRセンサーのxy方向の重み
int LINEposition_deg;                        // 角度

// サイドラインについて
const int SideLINE_right_pin_number = 8; // 右サイドのラインのピンを指定
const int SideLINE_left_pin_number = 9;  // 左サイドのラインのピンを指定
const int SideLINE_back_pin_number = 10; // 後サイドのラインのピンを指定

bool SideLINE_left, SideLINE_right, SideLINE_back; // サイドラインの状況

// ライン判定
int LINEsensor_see_judge_value;

void setup()
{
  // シリアルの初期設定
  Serial.begin(9600);                  // PCとの通信開始。ボートレートは9600とする。
  Serial1.begin(LINESEND_SERIALSPEED); // Teensyとの通信開始。ボートレートは9600とする。

  // マルチプレクサの初期設定
  MULTI16begin();
}

void loop()
{
  LINEsensor_see_judge_value = analogRead(0);

  LINEsensor_condition_count = 0; // 初期化
  LINEposition_of_x = 0;          // 初期化
  LINEposition_of_y = 0;          // 初期化
  for (int i = 0; i < 16; i++)
  {
    LINEsensor_value[i] = MULTI16read(LINEsensor_pin[i]);                               // LINEセンサーの状況を取得
    bool LINEsensor_now_condition = (LINEsensor_value[i] < LINEsensor_see_judge_value); // LINEセンサーは反応しているか
    if (LINEsensor_now_condition)
      LINEsensor_condition_count++; // 反応しているものがあるならばカウントアップ

    if (!LINEsensor_now_condition && LINEsensor_old_condition[i])
    {
      LINEsensor_value[i] = 10;        // 反応しているものとする
      LINEsensor_now_condition = true; // 反応しているものとする
    }

    LINEposition_of_x += cos(radians(i * 22.5)) * (1023 - LINEsensor_value[i]); // 重みを加算
    LINEposition_of_y += sin(radians(i * 22.5)) * (1023 - LINEsensor_value[i]); // 重みを加算

    LINEsensor_old_condition[i] = LINEsensor_now_condition; // 昔の状況を保存
  }

  if (LINEsensor_condition_count == 0)
  {
    // 1つも反応していないのならば
    LINEposition_deg = -1;
    for (int i = 0; i < 16; i++)
    {
      LINEsensor_old_condition[i] = false; // すべて反応していなかったものとする
    }
  }
  else
  {
    double LINEposition_deg_test = degrees(atan2(LINEposition_of_y, LINEposition_of_x)); // 角度を算出
    LINEposition_deg_test = (LINEposition_deg_test < 0) ? LINEposition_deg_test + 360 : LINEposition_deg_test;
    // LINEposition_deg_test = 360 - LINEposition_deg_test; // 反転させる
    LINEposition_deg = (int)LINEposition_deg_test;
  }

  SideLINE_right = (analogRead(SideLINE_right_pin_number) > 500); // サイドラインを読み取る
  SideLINE_left = (analogRead(SideLINE_left_pin_number) > 500);   // サイドラインを読み取る
  SideLINE_back = (analogRead(SideLINE_back_pin_number) > 500);   // サイドラインを読み取る

  char LINEposition_deg_char[4] = "";
  if (LINEposition_deg == -1)
  {
    strcat(LINEposition_deg_char, "---"); //-1なら"---"を代入
  }
  else
  {
    itoa(LINEposition_deg, LINEposition_deg_char, 10);
    while (strlen(LINEposition_deg_char) < 3)
    {
      char temp[4];
      strcpy(temp, LINEposition_deg_char); // tempにコピー
      strcpy(LINEposition_deg_char, "0");  // "0"をセット
      strcat(LINEposition_deg_char, temp); // "0"のあとにtempをつなぐ
    }
  }

  char SideLINE_right_char[2];
  itoa(SideLINE_right, SideLINE_right_char, 10);
  char SideLINE_left_char[2];
  itoa(SideLINE_left, SideLINE_left_char, 10);
  char SideLINE_back_char[2];
  itoa(SideLINE_back, SideLINE_back_char, 10);

  char Send_sentence[20] = "";
  strcat(Send_sentence, LINEposition_deg_char);
  strcat(Send_sentence, SideLINE_right_char);
  strcat(Send_sentence, SideLINE_left_char);
  strcat(Send_sentence, SideLINE_back_char);
  strcat(Send_sentence, "a");

  // Serial.write(Send_sentence);
  // Serial.write("\n");
  Serial1.write(Send_sentence);
  Serial1.flush(); // 空になるまで待つ

  delay(10);
}