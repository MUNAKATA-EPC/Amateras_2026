#pragma once

#include <Arduino.h>
#include "timer.hpp"
#include "packetManager.hpp"

bool ps3Init(HardwareSerial *serial, uint32_t baudrate);
void ps3StickAdjust(float left_adjust, float right_adjust);
void ps3Update();

// ステック
bool ps3LeftStickDetected();
bool ps3RightStickDetected();

int ps3LeftStickX();
int ps3LeftStickY();

int ps3RightStickX();
int ps3RightStickY();

int ps3LeftStickDeg();
int ps3RightStickDeg();

float ps3LeftStickDis();
float ps3RightStickDis();
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