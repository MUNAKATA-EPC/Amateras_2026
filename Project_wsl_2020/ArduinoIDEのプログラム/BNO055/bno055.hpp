#pragma once

#include <Arduino.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_Sensor.h>
#include <utility/imumaths.h>
#include <SPI.h>

#include "button.hpp"

#define BNO055_SAMPLERATE_DELAY_MS 10

Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire2);

int raw_deg;
int reset_deg;
int deg;

Button btn(10, Button_Type::BUTTON_PULLDOWN);

void bno_init() {

  delay(1000);

  if (!bno.begin()) {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while (1)
      ;
  }
  bno.setExtCrystalUse(true);
}

void bno_process() {
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);

  raw_deg = euler.x();
  deg = (int)(euler.x() - reset_deg + 360) % 360;

  Serial.print(analogRead(1));
  Serial.print(", ");
  Serial.println(deg);

  btn.loop();

  if (btn.isPushed()) {
    reset_deg = raw_deg;
  }
}

int bno_getdeg() {
  return deg;
}