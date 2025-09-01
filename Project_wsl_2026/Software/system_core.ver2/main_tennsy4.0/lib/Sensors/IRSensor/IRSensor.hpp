#pragma once

#include <Arduino.h>

class IRSensor
{
private:
    HardwareSerial *_serial;
    uint32_t _baudrate;
    uint8_t _frameHeader;

public:
    // コントラクタ
    IRSensor();

    // 関数
    void init(HardwareSerial *serial, uint32_t baudrate, uint8_t frameHeader); // 定義
    void update();                                                             // 更新・計算

    // 変数
    bool detected;
    int deg;
    int val;
    double dis;
};

extern IRSensor ir;