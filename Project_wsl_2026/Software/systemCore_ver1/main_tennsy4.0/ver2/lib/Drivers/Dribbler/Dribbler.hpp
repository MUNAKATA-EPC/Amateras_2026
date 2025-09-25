#pragma once

#include <Arduino.h>
#include <Servo.h>
#include "Functions.hpp"

class Dribbler
{
private:
    Servo _esc;
    uint8_t _pin;
    int _min, _max;

    Timer _setupTimer;
    bool _setup_flag = false;
    int _setup_count = 0;

public:
    // 関数・コントラクタ
    Dribbler(uint8_t pin, int min, int max);
    void begin();

    // 制御系
    void setup();
    void move(int power);

    // データの取得
    bool available() { return _setup_flag; }
};