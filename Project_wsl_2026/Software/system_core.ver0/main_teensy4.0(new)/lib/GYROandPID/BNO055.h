#ifndef MYLIB_BNO055_H

#define MYLIB_BNO055_H

#include <Adafruit_BNO055.h>
#include <Adafruit_Sensor.h>
#include <utility/imumaths.h>
#include <SPI.h>

// リセットしたときの角度を保存する変数
extern int BNO055Resetdeg;
// BNO055から直接の角度を保存する変数
extern int BNO055Normaldeg;
// 計算した角度を保存する変数
extern int BNO055Finaldeg;
// 計算した角度を保存する変数
extern int BNO055deg;

// BNO055を動かす関数
void BNO055begin();
// BNO055からの角度をリセットする関数
void BNO055reset();
// BNO055からの角度を読む関数
void BNO055get();

#endif