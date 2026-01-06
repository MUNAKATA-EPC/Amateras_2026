#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_Sensor.h>
#include "button.hpp"

Adafruit_BNO055 *_bno = nullptr;

int _deg_normal = 0; // BNO055からの生角度
int _deg_reset = 0;  // リセット時の角度
int _deg = 0;        // 補正後の角度格納用

bool bnoInit(TwoWire *wire, uint8_t address)
{
  if (_bno != nullptr)
  {
    delete _bno;
  }
  _bno = new Adafruit_BNO055(BNO055_ID, address, wire);

  Timer timer;
  timer.reset();
  bool success = _bno->begin(OPERATION_MODE_IMUPLUS);
  while (!success && timer.msTime() < 10000UL)
  {
    delay(10); // BNO055の通信開始待ち
  }
  _bno->setExtCrystalUse(true); // 外部水晶振動子使用

  return success;
}

void bnoUpdate(bool resetbtn)
{
  imu::Vector<3> euler = _bno->getVector(Adafruit_BNO055::VECTOR_EULER);
  _deg_normal = (int)euler.x(); // X軸の角度

  if (resetbtn) // リセットボタンが押されたら
    _deg_reset = _deg_normal;

  // 補正後の角度
  _deg = (int)((_deg_normal - _deg_reset + 360) % 360);

  _deg = (_deg > 180) ? _deg - 360 : _deg; // -180 ~ 180に変換
}

int bnoDeg() { return _deg; }

bool is_ok = false;

void setup()
{
  resetButton.init(1, INPUT_PULLDOWN); // ボタン設置
  is_ok = bnoInit(&Wire, 0x28);        // bno初期化

  pinMode(A0, OUTPUT);
  analogWriteResolution(10); // 10bitで出力
}

void loop()
{
  resetButton.update();
  bnoUpdate(resetButton.isPushing());

  uint32_t output = (uint32_t)(bnoDeg() + 360 + 180) % 360;
  output = (uint32_t)roundf(map(output, 0, 360, 400, 500)); // なぜか400~560の範囲でしか出せない
  analogWrite(A0, output);

  Serial.println(output);

  delay(10);
}