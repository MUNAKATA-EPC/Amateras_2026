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

// リセットスイッチのピン番号とモードを設定
void BNO055_set_resetpin(int pin, int pinmode);
// I2C定義・開始
void BNO055_init(TwoWire *wire, uint8_t adress);
// 更新
void BNO055_update();

// 計算した角度を出力
int get_BNO055_deg();

#endif