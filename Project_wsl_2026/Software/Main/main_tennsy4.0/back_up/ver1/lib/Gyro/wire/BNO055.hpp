#ifndef BNO055_HPP
#define BNO055_HPP

#include <Arduino.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_Sensor.h>
#include <utility/imumaths.h>
#include <SPI.h>
#include "button.hpp"
#include "toggle.hpp"
#include "timer.hpp"

// BNO055リセットボタンのピン番号とモードを設定
void BNO055_set_resetpin(int pin, int pinmode);

// I2C通信の初期化
void BNO055_init(TwoWire *wire, uint8_t address);

// データ更新
void BNO055_update();

// 計算した角度を取得
int get_BNO055_deg();

#endif
