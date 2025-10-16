#pragma once

#include <Arduino.h>
#include "timer.hpp"

class Button
{
private:
    uint8_t _pin = 0;                // ボタンのピン番号
    uint8_t _pinmode = INPUT_PULLUP; // ボタンのピンモード

    bool _pressed = false;
    bool _oldpressed = false;
    bool _released = false;

    Timer _pushingTimer; // 押されている時間計測用
    unsigned long _pushingTime = 0;

public:
    // 関数
    bool init(uint8_t pin, uint8_t pinmode);
    void update();

    bool isPushing();
    bool isReleased();

    unsigned long pushingTime();
    void pushingTimeReset();
};

extern Button enterButton; // 共通で使えるようにしておく
extern Button rightButton; // 共通で使えるようにしておく
extern Button leftButton;  // 共通で使えるようにしておく
extern Button resetButton; // 共通で使えるようにしておく