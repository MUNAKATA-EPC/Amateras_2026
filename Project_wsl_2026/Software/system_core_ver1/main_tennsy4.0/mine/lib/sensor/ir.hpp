/*SeeeduinoXiaoとSerial通信*/
#pragma once

#include <Arduino.h>
#include "timer.hpp"
#include "packet_manager.hpp"

bool irInit(HardwareSerial *serial, uint32_t baudrate);
void irUpdate();

bool irDetected();
int irDeg();
int irVal();
float irDis();

float irX();
float irY();