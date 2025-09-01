#pragma once

#include <Arduino.h>

class bitSensor
{
private:
    uint8_t _pin;

public:
    // コントラクタ
    bitSensor();

    // 関数
    void init(uint8_t pin); // 定義
    bool get();             // 取得
};

extern bitSensor catchsensor;