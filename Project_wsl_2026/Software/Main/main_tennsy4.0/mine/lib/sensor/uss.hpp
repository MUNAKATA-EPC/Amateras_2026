#pragma once

#include <Arduino.h>
#include "timer.hpp"
#include "packetManager.hpp"

bool ussInit(HardwareSerial *serial, uint32_t baudrate);
void ussUpdate();

// 0xFFFFでないかどうか
bool ussLeftDetected();
bool ussRightDetected();

// cm
int16_t ussLeftDis();
int16_t ussRightDis();

// cm/s
float ussLeftSpeed();
float ussRightSpeed();