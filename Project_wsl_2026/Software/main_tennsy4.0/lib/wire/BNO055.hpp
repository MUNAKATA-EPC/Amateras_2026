#ifndef BNO055_HPP
#define BNO055_HPP

#include <Arduino.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_Sensor.h>
#include <utility/imumaths.h>
#include <SPI.h>

class BNO055
{
private:
    Adafruit_BNO055 *_bno; // ポインタでとりあえず定義

    TwoWire *_wire;
    uint8_t _adress;

    int _reset_pin;
    int _reset_pinmode;

    int _normal_deg;
    int _reset_deg;
    int _deg;

public:
    BNO055(Adafruit_BNO055 *bno);           // 定義
    void setresetPin(int pin, int pinmode); // リセットピンの定義
    void begin();                           // シリアル開始
    void update();                          // 更新

    int getDeg(); // ジャイロの角度
};

#endif