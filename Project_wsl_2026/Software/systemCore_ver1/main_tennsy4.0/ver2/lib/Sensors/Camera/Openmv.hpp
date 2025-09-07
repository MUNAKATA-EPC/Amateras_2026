#pragma once

#include <Arduino.h>

class OpenmvData
{
private:
    bool _detected;
    int _deg;
    double _dis;

public:
    // データの取得
    bool detected() const { return _detected; }
    int deg() const { return _deg; }
    double dis() const { return _dis; }

    //_degと_disと_detectedをセットする
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
    // 関数・コントラクタ
    Openmv(HardwareSerial *serial, uint32_t baudrate, uint8_t frameHeader); // 定義
    void begin();                                                           // 開始
    void update();                                                          // 更新・計算

    OpenmvData yellowGoal;
    OpenmvData blueGoal;
    OpenmvData field;
};
