#pragma once

#include <Arduino.h>

class LineSensor
{
private:
    HardwareSerial *_serial;
    uint32_t _baudrate;
    uint8_t _frameHeader;

    double x;
    double y;

public:
    // コンストラクタ
    LineSensor();

    // 関数
    void init(HardwareSerial *serial, uint32_t baudrate, uint8_t frameHeader);
    int degCompute(bool *data); // -180~180
    void update();             // 更新・計算

    // 変数
    bool detected;
    bool sensor[19];

    bool sideRight;
    bool sideLeft;
    bool sideBack;

    int deg;
    double dis;
    int degMemory;
    bool sensorMemory[19];
};

extern LineSensor line;