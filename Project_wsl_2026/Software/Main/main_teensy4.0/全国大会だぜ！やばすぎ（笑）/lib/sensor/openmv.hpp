#pragma once

#include <Arduino.h>
#include "timer.hpp"
#include "packetManager.hpp"

bool openmvInit(HardwareSerial *serial, uint32_t baudrate);
void openmvUpdate();

bool fieldDetected();
float fieldDeg();

bool blueGoalDetected();
float blueGoalDeg();
float blueGoalDis();

bool yellowGoalDetected();
float yellowGoalDeg();
float yellowGoalDis();