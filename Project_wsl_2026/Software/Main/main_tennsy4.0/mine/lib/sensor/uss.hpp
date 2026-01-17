#pragma once

#include <Arduino.h>
#include "timer.hpp"
#include "packetManager.hpp"
#include "movementAverage.hpp"

bool ussInit(HardwareSerial *serial, uint32_t baudrate);
void ussUpdate();

// 0xFFFFでないかどうか
bool ussLeftDetected();
bool ussRightDetected();

// cm
uint16_t ussLeftDis();
uint16_t ussRightDis();

// cm/s
float ussLeftSpeed();
float ussRightSpeed();