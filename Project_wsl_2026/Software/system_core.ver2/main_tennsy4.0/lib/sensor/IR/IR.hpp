#pragma once

#include <Arduino.h>

class IR
{
private:
    HardwareSerial &_serial;
    uint32_t _baudrate;
    uint8_t _frameHeader;

public:
    IR();
    IR(HardwareSerial &serial, uint32_t baudrate, uint8_t frameHeader);

    void update(); // 更新・計算

    bool detected;
    int deg;
    int val;
    double dis;
};

extern IR ir;