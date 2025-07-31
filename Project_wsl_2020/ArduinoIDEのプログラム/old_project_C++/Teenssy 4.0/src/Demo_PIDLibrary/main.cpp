#include <Arduino.h>
#include "PIDv2.h"
#include "USE_BNO.h"
#include "USE_DSR1202.h"

void setup()
{
  // PCとのシリアル
  Serial.begin(115200);

  // シリアル1でDSRと通信する
  DSR1202begin(1);
  DSR1202motorGain(1.1, 0.78, 1.35, 1.03);

  // BNOとの通信を開始させる
  pinMode(22, INPUT_PULLDOWN);
  BNObegin(22);

  // PIDを開始する
  PIDv2begin(0.215, 0.01, 0.033, 16, 5);
}

void loop()
{
  int GyroPower = int(PIDv2(BNOread()));

  DSR1202move(-GyroPower, GyroPower, -GyroPower, GyroPower);

  delay(50);
}