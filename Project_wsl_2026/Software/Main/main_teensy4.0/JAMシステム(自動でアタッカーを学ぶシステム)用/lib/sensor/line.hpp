#pragma once

#include <Arduino.h>
#include "timer.hpp"
#include "packetManager.hpp"
#include "angleHelper.hpp"

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

// ライン制御に使える関数
bool lineRingFirstDetedcted();
int lineRingFirstDeg();
unsigned long lineRingDetectingTime();

// 反応の多い部分をチャンクごとに分けて返す(念のため32サイズ)
int lineChunkCount(int (&chunk_indexes)[32]);

// 白線の位置を検出する
enum LinePosi
{
    Tate_line,
    Yoko_line,
    Kado_line,
    Haji_line,
    Nai_line
};
LinePosi linePositionCheck();