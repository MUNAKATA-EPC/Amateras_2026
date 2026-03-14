/*SeeeduinoXiaoとSerial通信*/
#pragma once

#include <Arduino.h>
#include "timer.hpp"

bool lineInit(HardwareSerial *serial, uint32_t baudrate, uint8_t frameHeader);
void lineUpdate();

enum LineDataType
{
    RING,
    SIDE
};

bool lineRingDetected();
bool lineSideDetected();

bool lineSensorDetected(uint8_t index);

bool lineSideRightDetected();
bool lineSideLeftDetected();
bool lineSideBackDetected();

int lineRingDeg();
int lineSideDeg();

float lineRingDis();
float lineRingX();
float lineRingY();