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

float lineRingDeg();
float lineSideDeg();

float lineRingDis();
float lineRingX();
float lineRingY();

bool lineRingFirstDetedcted();
float lineRingFirstDeg();
unsigned long lineRingDetectingTime();

float lineRingLastDeg();                   // 一番最後に反応した角度
float lineRingLastDis();                   // 一番最後に反応した距離
unsigned long lineRingLastDetectingTime(); // 一番最後に反応したときからの時間

// チャンクを計算する関数 ( start_index + lengh ) % 16 = end_index が成り立つ
class LineChunk
{
public:
    int start_index = -1;
    int end_index = -1;
    float length = 0;
    float deg = 0xFF;
};
int lineChunkCount(LineChunk (&chunk)[16]);
