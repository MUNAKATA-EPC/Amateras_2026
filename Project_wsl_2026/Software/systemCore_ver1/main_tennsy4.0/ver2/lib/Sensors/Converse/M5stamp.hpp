#pragma once

#include <Arduino.h>

class M5stamp
{
private:
    HardwareSerial *_serial;
    uint32_t _baudrate;
    uint8_t _frameHeader;

    // 調整
    int _left_adjust;
    int _right_adjust;

    // 左ステック
    int _stick_lx;
    int _stick_ly;
    bool _leftstick_detected;
    int _leftstick_deg;
    double _leftstick_dis;
    // 右ステック
    int _stick_rx;
    int _stick_ry;
    bool _rightstick_detected;
    int _rightstick_deg;
    double _rightstick_dis;
    // ボタン（14個）
    uint16_t button_bitmask = 0;

public:
    // 関数・コントラクタ
    M5stamp(HardwareSerial *serial, uint32_t baudrate, uint8_t frameHeader); // 定義
    void begin(int left_adjust, int right_adjust);                           // 開始
    void update();                                                           // 更新・計算

    // データの取得
    // ステック
    enum StickData
    {
        LEFTSTICK,
        RIGHTSTICK
    };
    bool detected(StickData data)
    {
        switch (data)
        {
        case LEFTSTICK:
            return _leftstick_detected;
        case RIGHTSTICK:
            return _rightstick_detected;
        }
        return 0;
    }
    bool deg(StickData data)
    {
        switch (data)
        {
        case LEFTSTICK:
            return _leftstick_deg;
        case RIGHTSTICK:
            return _rightstick_deg;
        }
        return 0;
    }
    bool dis(StickData data)
    {
        switch (data)
        {
        case LEFTSTICK:
            return _leftstick_dis;
        case RIGHTSTICK:
            return _rightstick_dis;
        }
        return 0;
    }
    // ボタン
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
    bool button(ButtonType type)
    {
        return ((1 << (int)type) & button_bitmask) > 0; // (int)type分だけシフトした1との論理積が0よりも大きかったらそのbitは1
    }
};