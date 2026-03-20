#pragma once

#include <Arduino.h>
#include "timer.hpp"
#include "packetManager.hpp"
#include "movementAverage.hpp"
#include "angleHelper.hpp"

bool irInit(HardwareSerial *serial, uint32_t baudrate);
void irUpdate();

bool irDetected();
float irDeg();
float irVal();
float irDis();

float irX();
float irY();

float irXSpeed();
float irYSpeed();