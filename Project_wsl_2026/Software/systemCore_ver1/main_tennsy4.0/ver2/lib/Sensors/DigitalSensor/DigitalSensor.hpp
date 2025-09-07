#pragma once

#include <Arduino.h>

class DigitalSensor
{
private:
    uint8_t _pin;

public:
    //  関数・コントラクタ
    DigitalSensor(uint8_t pin); // 定義
    void begin();               // 開始

    enum ToggleSwitch
    {
        ON = HIGH,
        OFF = LOW
    };

    enum CatchSensor
    {
        Catch = HIGH,
        NONE = LOW
    };
    ToggleSwitch readToggle(); // 取得(トグルスイッチ版)
    CatchSensor readCatch();   // 取得(キャッチセンサー版)
};