#pragma once

#include <Arduino.h>

class LineSensor
{
private:
    HardwareSerial *_serial;
    uint32_t _baudrate;
    uint8_t _frameHeader;

    double _x;
    double _y;

    bool _detected;
    bool _sensor[19];

    bool _sideRight;
    bool _sideLeft;
    bool _sideBack;

    int _deg;
    double _dis;

    bool _sensorMemory[19];
    int _degMemory;

public:
    // 関数・コンストラクタ
    LineSensor(HardwareSerial *serial, uint32_t baudrate, uint8_t frameHeader); // 定義
    void begin();                                                               // 開始
    int degCompute(bool *data);                                                 // -180~180
    void update();                                                              // 更新・計算

    // データの取得
    bool detected() const { return _detected; }
    bool sensor(uint8_t index) const { return _sensor[index]; }

    bool sideRight() const { return _sideRight; }
    bool sideLeft() const { return _sideLeft; }
    bool sideBack() const { return _sideBack; }

    int deg() const { return _deg; }
    double dis() const { return _dis; }

    bool sensorMemory(uint8_t index) const
    {
        index = (index + 16) % 16;
        return _sensorMemory[index];
    }
    int degMemory() const { return _degMemory; }
};