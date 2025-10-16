/*OpenMVとSerial通信*/
#pragma once

#include <Arduino.h>

bool openmvInit(HardwareSerial *serial, uint32_t baudrate, uint8_t frameHeader);
void openmvUpdate();

enum CamDataType
{
    FIELD,
    YELLOWGOAL,
    BLUEGOAL
};
bool m5stampDetected(CamDataType data);
int m5stampDeg(CamDataType data);
int m5stampDis(CamDataType data);