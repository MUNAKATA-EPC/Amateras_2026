#include <Arduino.h>
#include "movement_average.hpp"
#include "multiplexer.hpp"

// #define NEW
#define OLD

const int head_byte = 0xAA; // 同期ヘッダー格納用

// IRセンサーのピン番号。前から反時計回りに指定
// センサー番号0がロボットの正面方向に対応していることを前提とします。
const int IRsensor_pin[16] = {0, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
// const double ir_gain[16] = {1, 0.9, 0.9, 0.9, 0.9, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
const double ir_gain[16] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

int IRsensor_value[16]; // IRセンサーの値を格納 (制約・ゲイン適用後, 0～1023)
int IRsensor_min_index; // IRセンサーで最も反応の小さいセンサーの配列番号 (ボールの方向を示す)

double IRball_of_x, IRball_of_y; // IRセンサーのxy方向の重み（ベクトル）
int IRball_deg;                  // ボール角度 (度: -180～180の範囲を想定)
int IRball_value;                // ボール強度/距離を示す値 (0～1023)

Multiplexer ir_mux;
Movement_average ir_ave[16];

void setup()
{
  Serial.begin(9600);
  Serial1.begin(115200);

  // マルチプレクサのピン設定 (実際の配線に合わせてください)
  ir_mux.set_pin(1, 2, 3, 4, 0, -1);
  // マルチプレクサの初期化
  ir_mux.init(100);

  for (int i = 0; i < 16; i++)
  {
    ir_ave[i].set(5); // 5個の平均をとる
  }
}

void loop()
{
  IRsensor_min_index = 0; // 最も小さい値を持つセンサーのインデックスを初期化

  // 1. 全センサーから値を取得し、移動平均とゲイン処理を行う
  for (uint8_t i = 0; i < 16; i++)
  {
    // マルチプレクサからセンサーの生の値を取得し、移動平均を計算
    ir_ave[i].add(ir_mux.read(IRsensor_pin[i]));
    int ave_value = ir_ave[i].output();

    if (ir_ave[i].cant_compute()) // 移動平均がまだ計算できない場合 (データ不足)
      ave_value = 1023;           // 無信号（最大値）を代入

    // int ave_value = ir_mux.read(IRsensor_pin[i]);

    // ゲインと調整値を適用し、四捨五入してから値を0～1023の範囲に制限
    // これにより、小数点以下の丸め誤差を防ぐ
    IRsensor_value[i] = constrain((int)round(ave_value), 0, 1023);

    // 最も反応の小さいセンサー（最も強い信号）を探す
    if (IRsensor_value[i] < IRsensor_value[IRsensor_min_index])
      IRsensor_min_index = i;
  }

  // 2. 最も反応の小さいセンサーとその周囲6個（計7個）のインデックスを決定
  int IRsensor_min_around_index[7] = {
      (IRsensor_min_index - 3 + 16) % 16,
      (IRsensor_min_index - 2 + 16) % 16,
      (IRsensor_min_index - 1 + 16) % 16,
      IRsensor_min_index,
      (IRsensor_min_index + 1 + 16) % 16,
      (IRsensor_min_index + 2 + 16) % 16,
      (IRsensor_min_index + 3 + 16) % 16,
  };

  // 3. ボールの方向と値の算出
  if (IRsensor_value[IRsensor_min_index] > 1000)
  {
    // センサー値がほぼ最大（ボールが見えない）の場合
    IRball_deg = 0xFF;
    IRball_value = 0xFF;

    IRball_of_x = 0.0; // 重み付けベクトルのX成分を初期化
    IRball_of_y = 0.0; // 重み付けベクトルのY成分を初期化
  }
  else
  {
    IRball_of_x = 0.0; // 重み付けベクトルのX成分を初期化
    IRball_of_y = 0.0; // 重み付けベクトルのY成分を初期化

    // 周囲7センサーの値を信号強度（1023.0 - 値）に変換し、ベクトル加算
    for (uint8_t i = 0; i < 7; i++)
    {
      // センサー角度を計算 (360度 / 16センサー = 22.5度/センサー)
      double angle_rad = radians(IRsensor_min_around_index[i] * 22.5);

      // (1023.0 - IRsensor_value)で重みを算出
      double weight = (1023.0 - IRsensor_value[IRsensor_min_around_index[i]] * ir_gain[IRsensor_min_around_index[i]]);

      IRball_of_x += cos(angle_rad) * weight; // 重みを加算
      IRball_of_y += sin(angle_rad) * weight; // 重みを加算
    }

    // 角度を計算し、度数に変換して丸める
    // atan2の結果は-πから+π（-180度から+180度）
    IRball_deg = (int)round(degrees(atan2(IRball_of_y, IRball_of_x)));

    // IRball_value（距離/強度）を計算
    // 浮動小数点数での除算を確実にするため、10.0を使用
    double IRball_of_x_for_value = IRball_of_x / 10.0;
    double IRball_of_y_for_value = IRball_of_y / 10.0;

    // ベクトルの大きさを求め、オフセット65を加算
    double IRball_value_sub = sqrt(IRball_of_x_for_value * IRball_of_x_for_value + IRball_of_y_for_value * IRball_of_y_for_value) + 65.0;

    // 値を0.0～1023.0の範囲に制限し、整数に変換
    IRball_value_sub = constrain(IRball_value_sub, 0.0, 1023.0);
    IRball_value = (int)IRball_value_sub;

#ifdef NEW
    IRball_value += 0;
#endif

#ifdef OLD
    IRball_value += 40;
#endif
  }

  // IRball_deg と IRball_value を int16_t 型に変換
  int16_t deg_to_send = (int16_t)IRball_deg;
  int16_t val_to_send = (int16_t)IRball_value;

  // 1. 未検出（0xFF）の場合、2バイトデータとして 0xFFとして送信する
  if (deg_to_send == 0xFF)
  {
    deg_to_send = 0xFF;
  }
  if (val_to_send == 0xFF)
  {
    val_to_send = 0xFF;
  }

  /* 4. 送信処理 (Serial1: Teensyなどとの通信) */
  Serial1.write(head_byte); // 同期ヘッダー (0xAA)

  // IRball_deg (2バイト送信: 下位バイト -> 上位バイトの順)
  Serial1.write((uint8_t)(deg_to_send & 0xFF));        // 下位バイト (low1)
  Serial1.write((uint8_t)((deg_to_send >> 8) & 0xFF)); // 上位バイト (high1)

  // IRball_value (2バイト送信: 下位バイト -> 上位バイトの順)
  Serial1.write((uint8_t)(val_to_send & 0xFF));        // 下位バイト (low2)
  Serial1.write((uint8_t)((val_to_send >> 8) & 0xFF)); // 上位バイト (high2)

  /* デバッグ用シリアル出力 */
  Serial.print(IRball_of_x);
  Serial.print(",");
  Serial.print(IRball_of_y);
  Serial.print("->");
  Serial.println(IRball_deg);

  delay(10);
}