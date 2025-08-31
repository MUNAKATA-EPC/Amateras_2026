//-----------------------------------------------------------------------
// Ultrasonic sensorをチェックする 
// 接続はUS1
// シリアルで書き出す
// 7segLEDは接続しないでください
//-----------------------------------------------------------------------
// Copyright (C) Future Robotics Technology Center All Right Reserved
//-----------------------------------------------------------------------
#include <Ultrasonic.h>

Ultrasonic ultrasonic(A3,A3);

void setup()
  {
  Serial.begin(9600);
  }

void loop()
  {
  float cmMsec;
  long microsec = ultrasonic.timing();

  cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM);
  Serial.print("MS: ");
  Serial.print(microsec);
  Serial.print(", CM: ");
  Serial.println(cmMsec);
  delay(100);
  }
