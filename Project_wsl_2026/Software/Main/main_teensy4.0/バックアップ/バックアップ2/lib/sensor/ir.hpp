/*SeeeduinoXiaoとSerial通信*/
#pragma once

#include <Arduino.h>
#include "timer.hpp"

bool irInit(HardwareSerial *serial, uint32_t baudrate, uint8_t frameHeader);
void irUpdate();

bool irDetected();
int irDeg();
int irVal();
float irDis();

float irX();
float irY();