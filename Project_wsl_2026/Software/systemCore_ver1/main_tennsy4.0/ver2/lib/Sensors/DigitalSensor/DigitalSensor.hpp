#pragma once

#include <Arduino.h>

// キャッチセンサーで使える
#define CATCH HIGH
#define NONE LOW
// トグルスイッチで使える
#define ON HIGH
#define OFF LOW

class DigitalSensor
{
private:
    uint8_t _pin;

public:
    //  関数・コントラクタ
    DigitalSensor(uint8_t pin); // 定義
    void begin();               // 開始
    bool read();                // 取得
};