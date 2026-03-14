#pragma once

#include <Arduino.h>

class DigitalSensor
{
private:
    uint8_t _pin = 0;

public:
    //  関数
    bool init(uint8_t pin); // 開始

    bool read(); // 取得
};

extern DigitalSensor catchSensor; // 共通で使えるようにしておく