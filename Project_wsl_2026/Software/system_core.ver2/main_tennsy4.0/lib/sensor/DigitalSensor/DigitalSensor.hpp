#pragma once

#include <Arduino.h>

class DigitalSensor
{
private:
    uint8_t _pin;

public:
    // コントラクタ
    DigitalSensor();

    // 関数
    void init(uint8_t pin); // 定義
    bool get();             // 取得
};

extern DigitalSensor catchsensor;