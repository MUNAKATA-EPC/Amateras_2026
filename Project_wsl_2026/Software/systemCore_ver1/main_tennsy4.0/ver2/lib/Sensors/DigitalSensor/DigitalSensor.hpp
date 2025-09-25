#pragma once

#include <Arduino.h>

class DigitalSensor
{
private:
    uint8_t _pin;

public:
    //  関数・コンストラクタ
    DigitalSensor(uint8_t pin); // 定義
    void begin();               // 開始

    bool read(); // 取得
};