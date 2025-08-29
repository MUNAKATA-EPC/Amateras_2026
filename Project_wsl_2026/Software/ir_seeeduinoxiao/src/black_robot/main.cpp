#include <Arduino.h>
#include "multiplexer.hpp"

/*黒ロボット用*/

const int head_byte = 0xAA; // 同期ヘッダー格納用

const int IRsensor_pin[16] = {0, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1}; // 前から反時計回りに指定

int IRsensor_value[16]; // IRセンサーの値を格納
int IRsensor_min_index; // IRセンサーで最も反応の小さいセンサーの配列番号

double IRball_of_x, IRball_of_y; // IRセンサーのxy方向の重み
int IRball_deg;                  // 角度
int IRball_value;                // 値

Multiplexer ir_mux;

void setup()
{
  Serial.begin(9600);
  Serial1.begin(115200);

  ir_mux.set_pin(1, 2, 3, 4, 5, -1); // マルチプレクサのピンを設定
  ir_mux.init(10);                   // マルチプレクサの初期化、
}

void loop()
{
  IRsensor_min_index = 0; // 初期化
  for (uint8_t i = 0; i < 16; i++)
  {
    IRsensor_value[i] = ir_mux.read(IRsensor_pin[i]);

    if (IRsensor_value[i] < IRsensor_value[IRsensor_min_index])
      IRsensor_min_index = i;
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
    IRball_value = -1;
  }
  else
  {
    IRball_of_x = 0; // 初期化
    IRball_of_y = 0; // 初期化
    for (uint8_t i = 0; i < 7; i++)
    {
      IRball_of_x += cos(radians(IRsensor_min_around_index[i] * 22.5)) * (1023.0 - IRsensor_value[IRsensor_min_around_index[i]]); // 重みを加算
      IRball_of_y += sin(radians(IRsensor_min_around_index[i] * 22.5)) * (1023.0 - IRsensor_value[IRsensor_min_around_index[i]]); // 重みを加算
    }

    double IRball_deg_sub = degrees(atan2(IRball_of_y, IRball_of_x)); // 角度を算出
    IRball_deg_sub = (IRball_deg_sub < 0) ? IRball_deg_sub + 360 : IRball_deg_sub;
    IRball_deg = (int)IRball_deg_sub;
    // IRball_distance = (int)(map(IRsensor_value[IRsensor_min_index], 0, 1023, 0, 200));
    // IRball_distance = pow(IRball_distance, 1.35);
    // IRball_distance = constrain(IRball_distance, 0, 1023);
    double IRball_of_x_for_value = IRball_of_x / 10;
    double IRball_of_y_for_value = IRball_of_y / 10;
    double IRball_value_sub = sqrt(IRball_of_x_for_value * IRball_of_x_for_value + IRball_of_y_for_value * IRball_of_y_for_value); // 距離を算出
    IRball_value_sub = constrain(IRball_value_sub, 0, 1023);
    IRball_value = (int)IRball_value_sub;
  }

  /*送信*/
  /*
  Serial1.write(head_byte);                       // teensyとの通信開始
  Serial1.write((uint8_t)IRball_deg);             // 2byteのデータなので下位の1byteのみ送信
  Serial1.write((uint8_t)(IRball_deg >> 8));      // 2byteのデータなので上位の1byteのみ送信
  Serial1.write((uint8_t)IRball_distance);        // 2byteのデータなので下位の1byteのみ送信
  Serial1.write((uint8_t)(IRball_distance >> 8)); // 2byteのデータなので上位の1byteのみ送信
  */
  Serial1.print("a");
  Serial1.print(IRball_deg);
  Serial1.print("b");
  Serial1.print(IRball_value);
  Serial1.print("c");
  Serial1.flush(); // 送信バッファがなくなるまで、つまり全て送信するまで待つ

  Serial.print(IRball_of_x);
  Serial.print(",");
  Serial.println(IRball_of_y);

  delay(10);
}
