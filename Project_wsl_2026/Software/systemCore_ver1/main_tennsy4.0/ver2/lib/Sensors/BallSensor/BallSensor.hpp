#pragma once

#include <Arduino.h>

class BallSensor
{
private:
    HardwareSerial *_serial;
    uint32_t _baudrate;
    uint8_t _frameHeader;

    bool _detected;
    int _deg;
    int _val;
    double _dis;

public:
    // 関数・コンストラクタ
    BallSensor(HardwareSerial *serial, uint32_t baudrate, uint8_t frameHeader); // 定義
    void begin();                                                               // 開始
    void update();                                                              // 更新・計算

    // データの取得
    bool detected() const { return _detected; }
    int deg() const { return _deg; }
    int val() const { return _val; }
    double dis() const { return _dis; }
};