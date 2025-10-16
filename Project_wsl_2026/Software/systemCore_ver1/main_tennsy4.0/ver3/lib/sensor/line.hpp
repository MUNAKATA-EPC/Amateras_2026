#pragma once

#include <Arduino.h>

bool lineInit(HardwareSerial *serial, uint32_t baudrate, uint8_t frameHeader);
void lineUpdate();

enum LineDataType
{
    RING,
    SIDE
};

bool lineDetected(LineDataType data);
bool lineSensorDetected(uint8_t index);

bool lineSideRightDetected();
bool lineSideLeftDetected();
bool lineSideBackDetected();

int lineDeg(LineDataType data);
double lineDis();

double lineRingX();
double lineRingY();