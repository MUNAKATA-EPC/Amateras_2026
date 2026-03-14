#pragma once

#include <Arduino.h>
#include "timer.hpp"
#include "packetManager.hpp"
#include "angleHelper.hpp"
#include "movementAverage.hpp"

bool lineInit(HardwareSerial *serial, uint32_t baudrate);
void lineUpdate();

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

bool lineRingFirstDetedcted();
int lineRingFirstDeg();
unsigned long lineRingDetectingTime();

int lineLastRingDeg();   // 一番最後に反応した角度
float lineLastRingDis(); // 一番最後に反応した距離

// チャンクを計算する関数 ( start_index + lengh ) % 16 = end_index が成り立つ
class LineChunk
{
public:
    int start_index = -1;
    int end_index = -1;
    int length = 0;
    int deg = 0xFF;
};
int lineChunkCount(LineChunk (&chunk)[16]);
