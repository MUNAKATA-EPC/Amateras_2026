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

    bool _ringDetected;
    bool _sideDetected;
    bool _sensor[19];

    bool _sideRight;
    bool _sideLeft;
    bool _sideBack;

    int _ringDeg;
    int _sideDeg;

    double _dis;

public:
    // 関数・コンストラクタ
    LineSensor(HardwareSerial *serial, uint32_t baudrate, uint8_t frameHeader); // 定義
    bool begin();                                                               // 開始
    int degCompute(bool *data);                                                 // -180~180
    void update();                                                              // 更新・計算

    // データの取得
    enum LineData
    {
        RING,
        SIDE
    };

    bool detected(LineData data) const
    {
        if (data == RING)
            return _ringDetected;

        return _sideDetected;
    }
    bool sensor(uint8_t index) const { return _sensor[index]; }

    bool sideRight() const { return _sideRight; }
    bool sideLeft() const { return _sideLeft; }
    bool sideBack() const { return _sideBack; }

    int deg(LineData data) const { 
        if (data == RING)
            return _ringDeg;

        return _sideDeg;
     }
    double dis() const { return _dis; }

    double x() const { return _x; }
    double y() const { return _y; }
};