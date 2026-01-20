#include <Arduino.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(0, INPUT);
}

bool flag = true;

void loop() {
  // put your main code here, to run repeatedly:
  if (flag) {
    int val = analogRead(0);
    Serial.println(val);
  }

  while (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    delay(1000);
    if (data == "stop") {
      Serial.print("\nstop\n");
      flag = false;
    } else if (data == "start") {
      Serial.print("\nstart\n");
      flag = true;
    } else {
      Serial.print("\nerror\n");
    }
    delay(1000);
  }

  delay(100);
}
