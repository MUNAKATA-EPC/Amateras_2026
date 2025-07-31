#include <Arduino.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_Sensor.h>
#include <utility/imumaths.h>
#include <SPI.h>
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x29, &Wire);

//定数
#define BNO055_SAMPLERATE_DELAY_MS 10
#define RESET_PIN 9
//変数
int raw_deg;
int reset_deg;
int deg;
//関数
void bno_init();
int bno_process();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(RESET_PIN,INPUT_PULLUP);
  bno_init();
}

void loop() {
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

  if (digitalRead(RESET_PIN) == LOW) {
    reset_deg = raw_deg;
  }

  return deg;
}