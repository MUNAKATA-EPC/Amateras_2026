#pragma once

#include <Arduino.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_Sensor.h>
#include <utility/imumaths.h>
#include <SPI.h>
#include "button.hpp"
#include "timer.hpp"

bool bnoInit(TwoWire *wire, uint8_t address);
void bnoUpdate(bool resetbtn);

int bnoDeg();