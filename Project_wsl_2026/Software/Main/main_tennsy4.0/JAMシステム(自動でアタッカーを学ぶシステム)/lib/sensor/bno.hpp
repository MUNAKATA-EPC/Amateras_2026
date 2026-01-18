#pragma once

#include <Arduino.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_Sensor.h>
#include <utility/imumaths.h>
#include <SPI.h>
#include "button.hpp"
#include "timer.hpp"
#include "movementAverage.hpp"

bool bnoInit(TwoWire *wire, uint8_t address);
void bnoUpdate(bool resetbtn, uint16_t uss_right_speed = 0xFFFF, uint16_t uss_left_speed = 0xFFFF);

int bnoDeg();
double bnoSpeedX();