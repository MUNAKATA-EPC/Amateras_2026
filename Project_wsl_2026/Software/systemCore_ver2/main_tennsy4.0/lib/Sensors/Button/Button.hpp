#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <Arduino.h>
#include "Timer/Timer.hpp"

class Button
{
private:
    uint8_t _pin = 0;                // ボタンのピン番号
    uint8_t _pinmode = INPUT_PULLUP; // ボタンのピンモード

    int8_t _count = 0; // ボタンが押されている回数格納用

    Timer _pushingTimer; // 押されている時間計測用
    unsigned long _pushingTime = 0;

public:
    // 関数
    void init(uint8_t pin, uint8_t pinmode);
    void update();

    bool isPushing();
    bool isReleased();

    unsigned long pushingTime();
    void pushingTimeReset();
};

/*ボタンの実態を定義*/
extern Button resetButton; // BNO055のリセットボタン用
extern Button enterButton; // 決定ボタン
extern Button rightButton; // 右ボタン
extern Button leftButton;  // 左ボタン

#endif
