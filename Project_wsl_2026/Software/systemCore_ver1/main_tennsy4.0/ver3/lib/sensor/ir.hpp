/*SeeeduinoXiaoとSerial通信*/
#pragma once

#include <Arduino.h>

bool irInit(HardwareSerial *serial, uint32_t baudrate, uint8_t frameHeader);
void irUpdate();

bool irDetected();
int irDeg();
int irVal();
double irDis();