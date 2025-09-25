#pragma once

#include <Arduino.h>

class M5stampData
{
private:
    int _x;
    int _y;
    bool _detected;
    int _deg;
    double _dis;

public:
    // データの所得
    int x() const { return _x; }
    int y() const { return _y; }
    bool detected() const { return _detected; }
    int deg() const { return _deg; }
    double dis() const { return _dis; }

    //_x,_yをセットする
    void setXY(int x, int y)
    {
        _x = x;
        _y = y;
    }
    //_degと_disと_detectedセットする
    void setAngle(int deg, double dis, bool detected)
    {
        _deg = deg;
        _dis = dis;
        _detected = detected;
    }
};

class M5stamp
{
private:
    HardwareSerial *_serial;
    uint32_t _baudrate;
    uint8_t _frameHeader;

    // 下位バイトと上位バイトを読み取りつなげる
    int16_t readSerial();

public:
    // 関数・コントラクタ
    M5stamp(HardwareSerial *serial, uint32_t baudrate, uint8_t frameHeader); // 定義
    void begin();                                                            // 開始
    void update();                                                           // 更新・計算

    // データの取得
    M5stampData LeftStick;
    M5stampData RightStick;
    enum ButtonType
    {
        UP,
        DOWN,
        LEFT,
        RIGHT,
        TRIANGLE,
        CIRCLE,
        CROSS,
        SQUARE,
        L1,
        L2,
        L3,
        R1,
        R2,
        R3
    };
    bool button(ButtonType name)
    {
        return false;
    }
};