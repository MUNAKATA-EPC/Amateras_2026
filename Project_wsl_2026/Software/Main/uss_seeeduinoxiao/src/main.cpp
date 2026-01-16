#include <Arduino.h>

// シリアルヘッダー
const int start_header = 0x55; // 同期ヘッダー格納用
const int end_header = 0xAA;   // 同期ヘッダー格納用

// 右超音波
#define RightTrigPin D10 // Trig Pin定義
#define RightEchoPin D9  // Echo Pin定義

// 左超音波
#define LeftTrigPin D4 // Trig Pin定義
#define LeftEchoPin D3 // Echo Pin定義

uint16_t readUssDis(uint8_t trig_pin, uint8_t echo_pin)
{
  // トリガ信号の生成
  digitalWrite(trig_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW);

  // 定数設定
  const float temperature = 25.0f;
  const float sound_speed = 331.5f + 0.6f * temperature; // [m/s]
  // タイムアウトを4m分とする
  const unsigned long timeout = 30000;

  // 距離測定
  uint32_t time = pulseIn(echo_pin, HIGH, timeout);

  if (time == 0UL)
  {
    return 0xFFFF;
  }

  // 計算（floatで計算してからuint16_tにキャスト）
  // 距離(cm) = 音速(m/s) * 100(cm/m) * 時間(us) / 1,000,000(us/s) / 2
  float dist_float = (sound_speed * (float)time) / 20000.0f;

  return (uint16_t)dist_float;
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
  // 計測開始時間記録
  uint32_t start_timer = micros();

  // 計測
  uint16_t right_distance = readUssDis(RightTrigPin, RightEchoPin);
  uint16_t left_distance = readUssDis(LeftTrigPin, LeftEchoPin);

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
  Serial1.flush();

  // 5ms間隔になるように調整
  uint32_t cost_time = micros() - start_timer;
  if (cost_time < 5000)
  {
    delayMicroseconds(5000 - cost_time);
  }
}