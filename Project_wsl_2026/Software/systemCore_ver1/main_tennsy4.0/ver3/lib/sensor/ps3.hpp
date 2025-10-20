/*M5Stamp-picoとSerial通信*/
/*M5stamp-picoはps3とBluetooth通信*/
#pragma once

#include <Arduino.h>
#include "timer.hpp"

bool ps3Init(HardwareSerial *serial, uint32_t baudrate, uint8_t frameHeader);
void ps3StickAdjust(int left_adjust, int right_adjust);
void ps3Update();

// ステック
bool ps3LeftStickDetected();
bool ps3RightStickDetected();

int ps3LeftStickDeg();
int ps3RightStickDeg();

double ps3LeftStickDis();
double ps3RightStickDis();
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