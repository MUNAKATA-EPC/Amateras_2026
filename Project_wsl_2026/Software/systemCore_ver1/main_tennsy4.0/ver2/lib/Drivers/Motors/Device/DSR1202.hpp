#pragma once

#include <Arduino.h>
#include "Sensors.hpp"

class DSR1202
{
private:
    HardwareSerial *_serial;
    uint32_t _baudrate;

    DigitalSensor   *_toggle;

public:
    // 関数・コンストラクタ
    DSR1202(HardwareSerial *serial, uint32_t baudrate);
    void begin(DigitalSensor *toggle);
    void stop();
    void move(int value_1ch, int value_2ch, int value_3ch, int value_4ch);
};