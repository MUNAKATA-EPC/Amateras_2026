/*SeeeduinoXiaoとSerial通信*/
#pragma once

#include <Arduino.h>
#include "timer.hpp"
#include "packetManager.hpp"

bool lineInit(HardwareSerial *serial, uint32_t baudrate);
void lineUpdate();

enum LineDataType
{
    RING,
    SIDE
};

bool lineIsAdjusting();
int lineValueToAdjust(uint8_t index);

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

// ライン制御に使える関数
bool lineRingFirstDetedcted();
int lineRingFirstDeg();
unsigned long lineRingDetectingTime();