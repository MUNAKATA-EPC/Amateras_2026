#include <Arduino.h>
// BNO関連のライブラリー
#include <Adafruit_BNO055.h>
#include <Adafruit_Sensor.h>
#include <utility/imumaths.h>
#include <SPI.h>
#include "USE_BNO.h"
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);

// 変数
int BNOreset_pin;
int BNOraw_deg;
int BNOreset_deg;
int BNOdeg;

void BNObegin(int pin_num)
{
  BNOreset_pin = pin_num;
  while (!bno.begin(OPERATION_MODE_IMUPLUS))
  {
  }
  bno.setExtCrystalUse(true);
}

int BNOread()
{
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);

  BNOraw_deg = euler.x();
  BNOdeg = (int)(euler.x() - BNOreset_deg + 360) % 360;

  if (digitalRead(BNOreset_pin) == HIGH)
  {
    BNOreset_deg = BNOraw_deg;
  }

  return BNOdeg;
}