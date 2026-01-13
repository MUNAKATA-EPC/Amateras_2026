#include <Arduino.h>

// シリアルヘッダー
const int start_header = 0x55; // 同期ヘッダー格納用
const int end_header = 0xAA;   // 同期ヘッダー格納用

// 右超音波
#define RightTrigPin D9  // Trig Pin定義
#define RightEchoPin D10 // Echo Pin定義

// 左超音波
#define LeftTrigPin D3 // Trig Pin定義
#define LeftEchoPin D4 // Echo Pin定義

unsigned long readUssDis(uint8_t trig_pin, uint8_t echo_pin)
{
  // トリガ信号パルス幅10μsを生成
  digitalWrite(trig_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW);

  // 定数設定
  const float temperature = 25.0f;                       // 気温[℃]
  const float sound_speed = 331.5f + 0.6f * temperature; // 音速[m/s]
  const unsigned long timeout = 30000;                   // タイムアウト時間30ms(4m前後)

  // 距離測定
  unsigned long time = pulseIn(echo_pin, HIGH, timeout); // EchoピンのHigh持続時間を測定
  if (time == 0)
  {
    return 0xFFFF; // タイムアウトの場合は遠すぎるのでuint16_tの最大値を返す
  }
  unsigned long distance = sound_speed * time / 2UL / 10000UL; // 音速[m/s]として距離の計算とcmへの換算

  return distance;
}

void setup()
{
  Serial.begin(9600);
  Serial1.begin(115200);

  // 超音波のピン設定
  pinMode(RightTrigPin, OUTPUT);
  pinMode(RightEchoPin, INPUT);

  pinMode(LeftTrigPin, OUTPUT);
  pinMode(LeftEchoPin, INPUT);
}

void loop()
{
  // 計測
  unsigned long right_distance = readUssDis(RightTrigPin, RightEchoPin);
  unsigned long left_distance = readUssDis(LeftTrigPin, LeftEchoPin);

  // デバッグ
  Serial.print("right uss:");
  Serial.print(right_distance);
  Serial.print("cm, ");

  Serial.print("left uss:");
  Serial.print(left_distance);
  Serial.println("cm");

  // Teensyに送信
  uint16_t right_distance_to_send = (uint16_t)right_distance;
  uint16_t left_distance_to_send = (uint16_t)left_distance;

  Serial1.write(start_header); // 通信開始

  // right_distance (2バイト送信: 下位バイト -> 上位バイトの順)
  Serial1.write((uint8_t)(right_distance_to_send & 0xFF));        // 下位バイト (low1)
  Serial1.write((uint8_t)((right_distance_to_send >> 8) & 0xFF)); // 上位バイト (high1)
  // left_distance (2バイト送信: 下位バイト -> 上位バイトの順)
  Serial1.write((uint8_t)(left_distance_to_send & 0xFF));        // 下位バイト (low2)
  Serial1.write((uint8_t)((left_distance_to_send >> 8) & 0xFF)); // 上位バイト (high2)

  Serial1.write(end_header); // 通信終了
}