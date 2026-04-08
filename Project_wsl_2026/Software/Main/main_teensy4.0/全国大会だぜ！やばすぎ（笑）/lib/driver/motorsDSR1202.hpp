#pragma once

#include <Arduino.h>

// #define LIMITS

class DSR1202
{
private:
    HardwareSerial *_serial;
    uint32_t _baudrate;
    uint8_t _toggle_pin = 0xFF;

    bool _is_toggle_on = false;

#ifndef LIMITS
    const uint8_t start_header = 0x55; // 同期ヘッダー格納用
    const uint8_t end_header = 0xAA;   // 同期ヘッダー格納用
    const int16_t stop_v = -32768;     // モーター停止数値

    void sendData(int16_t v1, int16_t v2, int16_t v3, int16_t v4);
#endif

public:
    // 関数・コンストラクタ
    DSR1202(HardwareSerial *serial, uint32_t baudrate);
    void begin();
    void setTogglePin(uint8_t pin, uint8_t pinmode);
    void stop();
    void move(int value_1ch, int value_2ch, int value_3ch, int value_4ch);

    bool isToggleOn();
};