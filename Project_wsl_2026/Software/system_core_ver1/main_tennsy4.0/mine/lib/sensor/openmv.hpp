/*OpenMVとSerial通信*/
#pragma once

#include <Arduino.h>
#include "timer.hpp"
#include "packetManager.hpp"

bool openmvInit(HardwareSerial *serial, uint32_t baudrate);
void openmvUpdate();

bool fieldDetected();
int fieldDeg();

bool blueGoalDetected();
int blueGoalDeg();
float blueGoalDis();

bool yellowGoalDetected();
int yellowGoalDeg();
float yellowGoalDis();