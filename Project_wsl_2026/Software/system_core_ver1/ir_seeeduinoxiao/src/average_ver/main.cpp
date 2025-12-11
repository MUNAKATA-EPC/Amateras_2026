#include <Arduino.h>
#include "movement_average.hpp"
#include "multiplexer.hpp"

const int start_header = 0x55; // 同期ヘッダー格納用
const int end_header = 0xAA;   // 同期ヘッダー格納用

// IRセンサーのピン番号。前から反時計回りに指定
// センサー番号0がロボットの正面方向に対応していることを前提とします。
const int IRsensor_pin[16] = {0, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
const float ir_gain[16] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};

int IRsensor_value[16]; // IRセンサーの値を格納
int IRsensor_min_index; // IRセンサーで最も反応の小さいセンサーの配列番号

float IRball_of_x, IRball_of_y; // IRセンサーのxy方向の重み（ベクトル）
int IRball_deg;                 // ボール角度
int IRball_val;                 // IRセンサーの値（距離）

Multiplexer ir_mux;
Movement_average ir_ave[16];
Movement_average x;
Movement_average y;

void setup()
{
  Serial.begin(9600);
  Serial1.begin(115200);

  // マルチプレクサのピン設定 (実際の配線に合わせてください)
  ir_mux.set_pin(1, 2, 3, 4, 0, -1);
  // マルチプレクサの初期化
  ir_mux.init(100);

  for (int i = 0; i < 16; i++)
    ir_ave[i].set(5); // 5個の平均をとる
  x.set(5);
  y.set(5);
}

void loop()
{
  IRsensor_min_index = 0; // 最も小さい値を持つセンサーのインデックスを初期化

  // データを移動平均を使って滑らかにする
  for (uint8_t i = 0; i < 16; i++)
  {
    ir_ave[i].add(ir_mux.read(IRsensor_pin[i]));
    int ave_value = ir_ave[i].output();

    if (ir_ave[i].cant_compute())
      ave_value = 1023;

    IRsensor_value[i] = constrain((int)round(ave_value), 0, 1023);

    if (IRsensor_value[i] < IRsensor_value[IRsensor_min_index])
      IRsensor_min_index = i;
  }

  // 最も反応の小さいセンサーとその周囲6個（計7個）のインデックスを決定
  int IRsensor_min_around_index[7] = {
      (IRsensor_min_index - 3 + 16) % 16,
      (IRsensor_min_index - 2 + 16) % 16,
      (IRsensor_min_index - 1 + 16) % 16,
      IRsensor_min_index,
      (IRsensor_min_index + 1 + 16) % 16,
      (IRsensor_min_index + 2 + 16) % 16,
      (IRsensor_min_index + 3 + 16) % 16,
  };

  // ボールの方向と値の算出
  IRball_of_x = 0.0f;
  IRball_of_y = 0.0f;

  if (IRsensor_value[IRsensor_min_index] > 1000)
  {
    IRball_deg = 0xFF;
    IRball_val = 0xFF;
  }
  else
  {
    for (uint8_t i = 0; i < 7; i++)
    {
      float angle_rad = radians(IRsensor_min_around_index[i] * 22.5f);
      float weight = (1023.0f - float(IRsensor_value[IRsensor_min_around_index[i]]) * ir_gain[IRsensor_min_around_index[i]]);

      IRball_of_x += cosf(angle_rad) * weight; // 重みを加算
      IRball_of_y += sinf(angle_rad) * weight; // 重みを加算
    }

    // ボールの座標にも移動平均を使ってみる（サンクス・トウ・ソエダ・センパイ）
    x.add((int)roundf(IRball_of_x * 1000.0f));
    y.add((int)roundf(IRball_of_x * 1000.0f));

    IRball_of_x = x.cant_compute() ? 0.0f : x.output();
    IRball_of_y = y.cant_compute() ? 0.0f : y.output();

    IRball_deg = (int)roundf(degrees(atan2(IRball_of_y, IRball_of_x)));
    IRball_val = IRsensor_value[IRsensor_min_around_index[3]]; // 最小のIRセンサーの値を距離とする

    /*float IRball_of_x_for_value = IRball_of_x / 10.0f;
    float IRball_of_y_for_value = IRball_of_y / 10.0f;

    float IRball_value_sub = sqrt(IRball_of_x_for_value * IRball_of_x_for_value + IRball_of_y_for_value * IRball_of_y_for_value) + 65.0f;

    IRball_value_sub = constrain(IRball_value_sub, 0.0f, 1023.0f);
    IRball_value = (int)IRball_value_sub;*/
  }

  int16_t deg_to_send = IRball_deg == 0xFF ? 0xFF : (int16_t)IRball_deg;
  int16_t val_to_send = IRball_val == 0xFF ? 0xFF : (int16_t)IRball_val;

  // Teensyに送信
  Serial1.write(start_header); // 通信開始

  // IRball_deg (2バイト送信: 下位バイト -> 上位バイトの順)
  Serial1.write((uint8_t)(deg_to_send & 0xFF));        // 下位バイト (low1)
  Serial1.write((uint8_t)((deg_to_send >> 8) & 0xFF)); // 上位バイト (high1)
  // IRball_value (2バイト送信: 下位バイト -> 上位バイトの順)
  Serial1.write((uint8_t)(val_to_send & 0xFF));        // 下位バイト (low2)
  Serial1.write((uint8_t)((val_to_send >> 8) & 0xFF)); // 上位バイト (high2)

  Serial1.write(end_header); // 通信終了

  delay(10);
}