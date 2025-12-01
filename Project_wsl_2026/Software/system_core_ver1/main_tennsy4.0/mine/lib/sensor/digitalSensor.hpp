#pragma once

#include <Arduino.h>
#include <cmath> // hsv計算用

/*　キャッチセンサーなどに使える万能関数　*/
class DigitalSensor
{
private:
    uint8_t _pin = 0;

public:
    //  関数
    bool init(uint8_t pin); // 開始

    bool read(); // 取得
};

extern DigitalSensor catchSensor; // 共通で使えるようにしておく

/*　フルカラーLED用関数　*/
class FullColorLed
{
private:
    uint8_t _r_pin = 0, _g_pin = 0, _b_pin = 0;

public:
    // 関数
    bool init(uint r_pin, uint8_t g_pin, uint8_t b_pin); // 開始

    void rgbLightUp(uint8_t r_val, uint8_t g_val, uint8_t b_val); // rgbで指定（0~255）
    void angleLightUp(int deg, uint8_t brightness);               // 色相環と明るさで指定
};

extern FullColorLed fullColorLed1; // 共通で使えるようにしておく