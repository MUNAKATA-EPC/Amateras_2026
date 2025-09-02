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

        // camToggle
        YELLOW_GOAL = HIGH,
        BLUE_GOAL = LOW,

        // motorToggle
        STOP = HIGH,
        START = LOW
    };

    // 関数
    void init(uint8_t pin); // 定義
    bool read();            // 取得
};

extern DigitalSensor catchSensor;
extern DigitalSensor camToggle;
extern DigitalSensor motorToggle;