#pragma once

#include <Arduino.h>

class DigitalSensor
{
private:
    uint8_t _pin;

public:
    // モード
    enum Mode
    {
        // catchSensor
        NONE = LOW,
        CATCH = HIGH,

        // motorToggle
        STOP = HIGH,
        START = LOW
    };

    // 関数・コントラクタ
    DigitalSensor(uint8_t pin); // 定義
    void begin();               // 開始
    bool read();                // 取得
};