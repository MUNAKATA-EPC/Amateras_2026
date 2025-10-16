#pragma once

#include <Arduino.h>

bool m5stampInit(HardwareSerial *serial, uint32_t baudrate, uint8_t frameHeader);
void m5stampStickAdjust(int left_adjust, int right_adjust);
void m5stampUpdate();

// ステック
enum StickDataType
{
    LEFTSTICK,
    RIGHTSTICK
};
bool m5stampDetected(StickDataType data);
int m5stampDeg(StickDataType data);
int m5stampDis(StickDataType data);
// ボタン
enum ButtonDataType
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
bool m5stampButtonIsPushing(ButtonDataType type);