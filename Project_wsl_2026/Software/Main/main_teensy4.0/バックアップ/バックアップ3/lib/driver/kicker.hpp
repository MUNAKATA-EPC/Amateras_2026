#pragma once

#include <Arduino.h>
#include "timer.hpp"

class Kicker
{
private:
    uint8_t _kickpin;
    uint8_t _chargepin;
    unsigned long _cooldownTime;
    Timer _kickTimer;

public:
    // 関数
    bool init(uint8_t kickpin, uint8_t chargepin, unsigned long cooldownTime); // 開始
    void kick(bool signal);                                                     // キックする
};

extern Kicker kicker1; // 共通で使えるようにしておく