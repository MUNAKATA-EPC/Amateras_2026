#pragma once

#include <Arduino.h>

class Openmv
{
private:
    HardwareSerial *_serial;
    uint32_t _baudrate;
    uint8_t _frameHeader;

    // コート
    bool _field_detected;
    int _field_deg;
    // 青ゴール
    bool _bluegoal_detected;
    int _bluegoal_deg;
    double _bluegoal_dis;
    // 黄ゴール
    bool _yellowgoal_detected;
    int _yellowgoal_deg;
    double _yellowgoal_dis;

public:
    // 関数・コントラクタ
    Openmv(HardwareSerial *serial, uint32_t baudrate, uint8_t frameHeader); // 定義
    void begin();                                                           // 開始
    void update();                                                          // 更新・計算

    // データの所得
    enum CamData
    {
        FIELD,
        YELLOWGOAL,
        BLUEGOAL
    };
    bool detected(CamData data)
    {
        switch (data)
        {
        case FIELD:
            return _field_detected;
        case YELLOWGOAL:
            return _yellowgoal_detected;
        case BLUEGOAL:
            return _bluegoal_detected;
        }
        return 0;
    }
    int deg(CamData data)
    {
        switch (data)
        {
        case FIELD:
            return _field_deg;
        case YELLOWGOAL:
            return _yellowgoal_deg;
        case BLUEGOAL:
            return _bluegoal_deg;
        }
        return 0;
    }
    int dis(CamData data)
    {
        switch (data)
        {
        case FIELD:
            return 0;
        case YELLOWGOAL:
            return _yellowgoal_dis;
        case BLUEGOAL:
            return _bluegoal_dis;
        }
        return 0;
    }
};
