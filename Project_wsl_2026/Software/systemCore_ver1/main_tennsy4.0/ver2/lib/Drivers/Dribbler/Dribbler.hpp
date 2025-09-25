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

public:
    // 関数・コントラクタ
    Dribbler::Dribbler(uint8_t pin, int min, int max);
    void begin();

    // 制御系
    void process();
    void move(int power);

    // データの取得
    bool available() { return _setup_flag; }
};