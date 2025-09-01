#pragma once

#include <Arduino.h>

class DigitalSensor
{
private:
    uint8_t _pin;

public:
    // コントラクタ
    DigitalSensor();

    // モード
    enum Mode
    {
        // catchSensor
        NONE = false,
        CATCH = true,

        // camToggle
        YELLOW_GOAL = true,
        BLUE_GOAL = false,

        // motorToggle
        STOP = true,
        START = false
    };

    // 関数
    void init(uint8_t pin); // 定義
    bool read();            // 取得
};

extern DigitalSensor catchSensor;
extern DigitalSensor camToggle;
extern DigitalSensor motorToggle;