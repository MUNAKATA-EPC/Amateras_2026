#include <Arduino.h>
#include "multiplexer.hpp"

/*黒ロボット用*/

const int head_byte = 0xAA; // 同期ヘッダー格納用

// IRセンサーのピン番号。前から反時計回りに指定
// センサー番号0がロボットの正面方向に対応していることを前提とします。
const int IRsensor_pin[16] = {0, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};

const double ir_gain = 1;
const double ir_adjust = 0;

int IRsensor_value[16]; // IRセンサーの値を格納 (制約・ゲイン適用後, 0～1023)
int IRsensor_min_index; // IRセンサーで最も反応の小さいセンサーの配列番号 (ボールの方向を示す)

double IRball_of_x, IRball_of_y; // IRセンサーのxy方向の重み（ベクトル）
int IRball_deg;                  // ボール角度 (度: -180～180の範囲を想定)
int IRball_value;                // ボール強度/距離を示す値 (0～1023)

Multiplexer ir_mux;

void setup()
{
  Serial.begin(9600);
  Serial1.begin(115200);

  // マルチプレクサのピン設定 (実際の配線に合わせてください)
  ir_mux.set_pin(1, 2, 3, 4, 5, -1);
  // マルチプレクサの初期化、ここではアナログピン10を使用
  ir_mux.init(10);
}

 // 16個のセンサーそれぞれの移動平均を計算する
 // index センサーの配列番号 (0～15)
 // new_value 新しい測定値 (0～1023)
 // int 移動平均値。有効なデータがない場合は-1を返す。
int GetMovementAverage(int index, int new_value)
{
  // history_size: 過去の値を保存する数 (移動平均の期間)
  const int history_size = 4;

  // static変数はプログラム実行中に一度だけ初期化され、値を保持し続ける
  // 全要素を-1（無効値）で初期化
  static int movement_history[16][history_size] = {-1};
  static int movement_index[16] = {0};

  // 1. 新しい値を循環バッファに保存
  movement_history[index][movement_index[index]] = new_value;

  // 2. インデックスを更新 (history_sizeの範囲で循環)
  movement_index[index] = (movement_index[index] + 1) % history_size;

  // 3. 移動平均の計算
  int sum = 0, count = 0;
  for (int i = 0; i < history_size; i++)
  {
    if (movement_history[index][i] != -1) // -1（無効値）は平均には使わない
    {
      count++;
      sum += movement_history[index][i];
    }
  }

  if (count == 0) // 有効な値がない場合は-1を返す
    return -1;

  // 厳密な平均を計算するため、浮動小数点数で計算し、四捨五入する
  return (int)round((double)sum / count);
}

void loop()
{
  IRsensor_min_index = 0; // 最も小さい値を持つセンサーのインデックスを初期化

  // 1. 全センサーから値を取得し、移動平均とゲイン処理を行う
  for (uint8_t i = 0; i < 16; i++)
  {
    // マルチプレクサからセンサーの生の値を取得し、移動平均を計算
    int ave_value = GetMovementAverage(i, ir_mux.read(IRsensor_pin[i]));

    if (ave_value == -1) // 移動平均がまだ計算できない場合 (データ不足)
      ave_value = 1023;  // 無信号（最大値）を代入

    // ゲインと調整値を適用し、四捨五入してから値を0～1023の範囲に制限
    // これにより、小数点以下の丸め誤差を防ぐ
    IRsensor_value[i] = constrain((int)round(ave_value * ir_gain + ir_adjust), 0, 1023);

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
    IRball_deg = -1;
    IRball_value = -1;
  }
  else
  {
    IRball_of_x = 0; // 重み付けベクトルのX成分を初期化
    IRball_of_y = 0; // 重み付けベクトルのY成分を初期化

    // 周囲7センサーの値を信号強度（1023.0 - 値）に変換し、ベクトル加算
    for (uint8_t i = 0; i < 7; i++)

    {
      // センサー角度を計算 (360度 / 16センサー = 22.5度/センサー)
      double angle_rad = radians(IRsensor_min_around_index[i] * 22.5);

      // (1023.0 - IRsensor_value)で信号強度（重み）を算出
      double weight = (1023.0 - IRsensor_value[IRsensor_min_around_index[i]]);

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

    // ベクトルの大きさ（マグニチュード）を求め、オフセット65を加算
    double IRball_value_sub = sqrt(IRball_of_x_for_value * IRball_of_x_for_value + IRball_of_y_for_value * IRball_of_y_for_value) + 65.0;

    // 値を0.0～1023.0の範囲に制限し、整数に変換
    IRball_value_sub = constrain(IRball_value_sub, 0.0, 1023.0);
    IRball_value = (int)IRball_value_sub;
  }

  /* 4. 送信処理 (Serial1: Teensyなどとの通信) */
  Serial1.write(head_byte);
  // 同期ヘッダー (0xAA)

  // IRball_deg (2バイト送信, 下位バイト→上位バイトの順)
  Serial1.write((uint8_t)IRball_deg);

  Serial1.write((uint8_t)(IRball_deg >> 8));

  // IRball_value (2バイト送信, 下位バイト→上位バイトの順)
  Serial1.write((uint8_t)IRball_value);

  Serial1.write((uint8_t)(IRball_value >> 8));

  /* デバッグ用シリアル出力 (Serial: PCとの通信) */
  Serial.print(IRball_of_x);
  Serial.print(",");
  Serial.println(IRball_of_y);

  delay(10);
}