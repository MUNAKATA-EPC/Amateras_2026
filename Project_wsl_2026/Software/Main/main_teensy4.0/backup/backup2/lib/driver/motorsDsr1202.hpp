#pragma once

#include <Arduino.h>

class DSR1202
{
private:
    HardwareSerial *_serial;
    uint32_t _baudrate;

public:
    // 関数・コンストラクタ
    DSR1202(HardwareSerial *serial, uint32_t baudrate);
    void begin();
    void stop();
    void move(int value_1ch, int value_2ch, int value_3ch, int value_4ch);
};