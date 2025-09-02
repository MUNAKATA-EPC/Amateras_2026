#pragma once

#include <Arduino.h>

class CamData
{
private:
    bool _detected;
    int16_t _deg;
    double _dis;

public:
    // データの取得
    bool detected() const { return _detected; }
    int16_t deg() const { return _deg; }
    double dis() const { return _dis; }

    //_degと_disをセットする
    void set(int deg, double dis)
    {
        _deg = deg;
        _dis = dis;
        _detected = (deg != -1);
    }
};

class Openmv
{
private:
    HardwareSerial *_serial;
    uint32_t _baudrate;
    uint8_t _frameHeader;

    // 下位バイトと上位バイトを読み取りつなげる
    int16_t readSerial();

public:
    // 関数
    void init(HardwareSerial *serial, uint32_t baudrate, uint8_t frameHeader); // 定義
    void update();                                                             // 更新・計算

    CamData yellowGoal;
    CamData blueGoal;
    CamData field;
};

extern Openmv cam;
