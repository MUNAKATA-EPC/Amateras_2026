/*OpenMVとSerial通信*/
#pragma once

#include <Arduino.h>
#include "timer.hpp"

bool openmvInit(HardwareSerial *serial, uint32_t baudrate, uint8_t frameHeader);
void openmvUpdate();

bool openmvFieldDetected();
int openmvFieldDeg();

bool openmvBlueGoalDetected();
int openmvBlueGoalDeg();
double openmvBlueGoalDis();

bool openmvYellowGoalDetected();
int openmvYellowGoalDeg();
double openmvYellowGoalDis();