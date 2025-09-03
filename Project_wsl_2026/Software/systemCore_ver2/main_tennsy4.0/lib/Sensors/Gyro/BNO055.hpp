#pragma once

#include <Arduino.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_Sensor.h>
#include <utility/imumaths.h>
#include <SPI.h>
#include "Button/Button.hpp"
#include "Timer/Timer.hpp"

class BNO055
{
private:
    Adafruit_BNO055 *_bno = nullptr;
    TwoWire *_wire;
    uint8_t _address;
    Button *_resetbtn;

    int _degNormal; // BNO055からの生角度
    int _degReset;  // リセット時の角度
    int _deg;       // 計算し算出した角度

public:
    // 関数・コントラクタ
    BNO055(TwoWire *wire, uint8_t address, Button *resetbtn); // 定義
    void begin();                                             // 開始
    void update();                                            // 更新・計算

    // データの取得
    int deg() const { return _deg; }
};