#pragma once

#include <Arduino.h>

class Kicker
{
private:
    uint8_t _kickpin;
    uint8_t _chargepin;

public:
    // 関数・コントラクタ
    Kicker(uint8_t kickpin, uint8_t chergepin); // 定義
    void begin(unsigned long cooldownTime);     // 開始
    void kick(bool signal);                     // キックする
};