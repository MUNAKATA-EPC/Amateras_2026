#pragma once

#include <Arduino.h>
#include "Functions.hpp"

class Kicker
{
private:
    uint8_t _kickpin;
    uint8_t _chargepin;
    unsigned long _cooldownTime;
    Timer _kickTimer;

public:
    // 関数・コンストラクタ
    Kicker(uint8_t kickpin, uint8_t chargepin); // 定義
    void begin(unsigned long cooldownTime);     // 開始
    void kick(bool signal);                     // キックする
};