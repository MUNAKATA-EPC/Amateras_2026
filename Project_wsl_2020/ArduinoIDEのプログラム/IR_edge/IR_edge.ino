float IR_deg;

const int numSensors = 8;
int sensorPins[numSensors] = { A1, A2, A3, A4, A5, A8, A9, A10 };
int sensorValues[numSensors];

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
}

void loop() {
  int minValue = 1000;  // 最大のアナログ値
  int minPin = -1;

  int i;
  for (i = 0; i < numSensors; i++) {
    sensorValues[i] = analogRead(sensorPins[i]);
    if (sensorValues[i] < minValue) {
      minValue = sensorValues[i];
      minPin = i;
    }
  }

  if (minPin != -1) {
    int rightPin, leftPin;
    rightPin = minPin - 1;
    if (rightPin < 0) rightPin = 7;
    leftPin = minPin + 1;
    if (rightPin > 7) leftPin = 0;

    IR_deg = 45 * sensorValues[leftPin] / (sensorValues[rightPin] + sensorValues[leftPin]);
    IR_deg += 45 * minPin;

    IR_deg -= 22.5;
    if (IR_deg < 0) IR_deg = 360 + IR_deg;
  } else {
    IR_deg = -1;
  }

  Serial.print(IR_deg);
  Serial.println("deg");
  Serial1.print(IR_deg);
  Serial1.print('\n');  // 改行を追加して送信
  delay(100);
}
