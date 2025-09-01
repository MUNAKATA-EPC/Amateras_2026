#pragma once

#include <Arduino.h>

class LINE
{
private:
    HardwareSerial *_serial;
    uint32_t _baudrate;
    uint8_t _frameHeader;

    double x;
    double y;

public:
    // コンストラクタ
    LINE();

    // 関数
    void init(HardwareSerial *serial, uint32_t baudrate, uint8_t frameHeader);
    int computedeg(bool *data); // -180~180
    void update();             // 更新・計算

    // 変数
    bool detected;
    bool sensors[19];

    bool side_right;
    bool side_left;
    bool side_back;

    int deg;
    double dis;
    int memory_deg;
    bool memory_sensors[19];
};

extern LINE line;