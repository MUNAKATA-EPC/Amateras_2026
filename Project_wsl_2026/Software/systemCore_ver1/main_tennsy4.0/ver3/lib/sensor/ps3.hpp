/*m5stamp-picoを使ってps3とBluetooth通信*/
#pragma once

#include <Arduino.h>

bool ps3Init(HardwareSerial *serial, uint32_t baudrate, uint8_t frameHeader);
void ps3StickAdjust(int left_adjust, int right_adjust);
void ps3Update();

// ステック
enum StickDataType
{
    LEFTSTICK,
    RIGHTSTICK
};
bool ps3Detected(StickDataType data);
int ps3Deg(StickDataType data);
int ps3Dis(StickDataType data);
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
bool ps3ButtonIsPushing(ButtonDataType type);