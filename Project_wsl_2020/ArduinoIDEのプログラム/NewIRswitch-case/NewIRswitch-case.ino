const int numSensors = 8;
const int sensorPins[numSensors] = { A3, A4, A5, A6, A7, A8, A9, A10 };
int sensorValues[numSensors];

int minPin[2];
int dist, DAC_Value;

void setup() {
  // put your setup code here, to run once:
  pinMode(0, OUTPUT);
  analogWriteResolution(10);
  Serial.begin(9600);
  for (int i = 0; i < numSensors; i++) {
    pinMode(sensorPins[i], INPUT);
  }
  pinMode(6, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  for (int i = 0; i < numSensors; i++) {
    sensorValues[i] = analogRead(sensorPins[i]);
  }

  dist = 1000;
  minPin[0] = -1;
  minPin[1] = dist;
  for (int i = 0; i < numSensors; i++) {
    if (sensorValues[i] < dist) {
      dist = sensorValues[i];
      minPin[0] = i;
      minPin[1] = sensorValues[i];
    }
  }

  if (minPin[0] == -1) {
    DAC_Value = 1023;
  } else {
    DAC_Value = minPin[0] * 140;
    analogWrite(0, DAC_Value);
  }
  if (dist < analogRead(2)) {
    analogWrite(1, 1023);
  } else {
    analogWrite(1, 0);
  }

  Serial.print(minPin[0]);
  Serial.print(" ->  Value: ");
  Serial.print(minPin[1]);
  Serial.print(", NEAR?: ");
  Serial.print(dist < analogRead(2));
  Serial.print(", DATA: ");
  Serial.println(DAC_Value);

  delay(100);
}
