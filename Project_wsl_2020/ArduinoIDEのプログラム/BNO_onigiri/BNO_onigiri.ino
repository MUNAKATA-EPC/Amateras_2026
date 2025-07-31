#pragma once

#include <Arduino.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_Sensor.h>
#include <utility/imumaths.h>
#include <SPI.h>

#define BNO055_SAMPLERATE_DELAY_MS 10

Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x29, &Wire);

int raw_deg;
int reset_deg;
int deg;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(6, INPUT_PULLUP);
  pinMode(0, OUTPUT);
  //analogWriteResolution(10);
  bno_init();
}

void loop() {
  analogWrite(0, bno_process() * 5 / 3.3);
  Serial.println(bno_process());
  delay(10);
}


//function


void bno_init() {
  while(!bno.begin(OPERATION_MODE_IMUPLUS)){}
  bno.setExtCrystalUse(true);
}

int bno_process() {
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);

  raw_deg = euler.x();
  deg = (int)(euler.x() - reset_deg + 360) % 360;

  if (digitalRead(6) == LOW) {
    reset_deg = raw_deg;
  }

  return deg;
}