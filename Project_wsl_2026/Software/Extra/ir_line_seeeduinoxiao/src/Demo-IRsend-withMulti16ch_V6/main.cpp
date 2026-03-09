#include <Arduino.h>
#include "MyLib-Multi16ch.h"
#include "MySettings.h"

const int IRsensor_pin[16] = {0, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1}; // 前から反時計回りに指定

int IRsensor_value[16]; // IRセンサーの値を格納
int IRsensor_min_index; // IRセンサーで最も反応の小さいセンサーの配列番号

double IRball_of_x, IRball_of_y; // IRセンサーのxy方向の重み
int IRball_deg;                  // 角度
int IRball_distance;             // 距離

void setup()
{
  Serial.begin(9600);
  Serial1.begin(IRSEND_SERIALSPEED);

  MULTI16begin(); // マルチプレクサ初期化
}

void loop()
{
  IRsensor_min_index = 0; // 初期化
  for (int i = 0; i < 16; i++)
  {
    IRsensor_value[i] = MULTI16read(IRsensor_pin[i]);

    if (IRsensor_value[i] < IRsensor_value[IRsensor_min_index])
    {
      IRsensor_min_index = i;
    }
  }

  int IRsensor_min_around_index[7] = {
      (IRsensor_min_index - 3 + 16) % 16,
      (IRsensor_min_index - 2 + 16) % 16,
      (IRsensor_min_index - 1 + 16) % 16,
      IRsensor_min_index,
      (IRsensor_min_index + 1 + 16) % 16,
      (IRsensor_min_index + 2 + 16) % 16,
      (IRsensor_min_index + 3 + 16) % 16,
  }; // 小さいIRセンサーから周り3個の配列番号を特定

  if (IRsensor_value[IRsensor_min_index] > 1000)
  {
    IRball_deg = -1;
    IRball_distance = -1;
  }
  else
  {
    IRball_of_x = 0; // 初期化
    IRball_of_y = 0; // 初期化
    for (int i = 0; i < 7; i++)
    {
      IRball_of_x += cos(radians(IRsensor_min_around_index[i] * 22.5)) * (1023.0 - IRsensor_value[IRsensor_min_around_index[i]]); // 重みを加算
      IRball_of_y += sin(radians(IRsensor_min_around_index[i] * 22.5)) * (1023.0 - IRsensor_value[IRsensor_min_around_index[i]]); // 重みを加算
    }

    double IRball_deg_test = degrees(atan2(IRball_of_y, IRball_of_x)); // 角度を算出
    IRball_deg_test = (IRball_deg_test < 0) ? IRball_deg_test + 360 : IRball_deg_test;
    IRball_deg = (int)IRball_deg_test;
    // IRball_distance = (int)(map(IRsensor_value[IRsensor_min_index], 0, 1023, 0, 200));
    double IRball_of_x_for_distance = IRball_of_x / 10.0;
    double IRball_of_y_for_distance = IRball_of_y / 10.0;
    double IRball_distance_test = sqrt(IRball_of_x_for_distance * IRball_of_x_for_distance + IRball_of_y_for_distance * IRball_of_y_for_distance); // 距離を算出
    IRball_distance = (int)map(IRball_distance_test, 0, 500, 500, 0);
    Serial.println(IRball_distance);
  }

  char IRball_deg_char[4] = "";
  char IRball_distance_char[4] = "";
  if (IRball_deg == -1)
  {
    strcat(IRball_deg_char, "---"); //-1なら"---"を代入

    strcat(IRball_distance_char, "---"); //-1なら"---"を代入
  }
  else
  {
    itoa(IRball_deg, IRball_deg_char, 10);
    while (strlen(IRball_deg_char) < 3)
    {
      char temp[4];
      strcpy(temp, IRball_deg_char); // tempにコピー
      strcpy(IRball_deg_char, "0");  // "0"をセット
      strcat(IRball_deg_char, temp); // "0"のあとにtempをつなぐ
    }

    itoa(IRball_distance, IRball_distance_char, 10);
    while (strlen(IRball_distance_char) < 3)
    {
      char temp[4];
      strcpy(temp, IRball_distance_char); // tempにコピー
      strcpy(IRball_distance_char, "0");  // "0"をセット
      strcat(IRball_distance_char, temp); // "0"のあとにtempをつなぐ
    }
  }

  char Send_sentence[20] = "";
  strcat(Send_sentence, IRball_deg_char);
  strcat(Send_sentence, IRball_distance_char);
  strcat(Send_sentence, "a");

  // Serial.write(Send_sentence);
  Serial1.write(Send_sentence);

  delay(50);
}
