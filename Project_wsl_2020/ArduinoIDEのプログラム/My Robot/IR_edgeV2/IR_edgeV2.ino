const int numSensors = 8;
const int sensorPins[numSensors] = { A7, A8, A9, A10, A3, A4, A5, A6 };
int sensorValues[numSensors];
float angles[numSensors];
float x[numSensors];
float y[numSensors];
float R = 65;  // センサーの円の半径（適宜変更してください）

float sumX = 0;
float sumY = 0;
float sumValues = 0;

void setup() {
  pinMode(0, OUTPUT);
  analogWriteResolution(10);
  pinMode(1, OUTPUT);
  pinMode(2, INPUT);
  Serial.begin(9600);
  for (int i = 0; i < numSensors; i++) {
    pinMode(sensorPins[i], INPUT);
    angles[i] = i * (360.0 / numSensors);  // 各センサーの角度を計算
  }
}

void loop() {
  for (int i = 0; i < numSensors; i++) {
    sensorValues[i] = analogRead(sensorPins[i]);
    // 正規化
    sensorValues[i] = map(sensorValues[i], 0, 1023, 0, 100);
    x[i] = R * cos(radians(angles[i]));
    y[i] = R * sin(radians(angles[i]));
  }

  // IRボールの角度を計算
  sumX = 0;
  sumY = 0;
  sumValues = 0;
  for (int i = 0; i < numSensors; i++) {
    sumX += sensorValues[i] * x[i];
    sumY += sensorValues[i] * y[i];
    sumValues += sensorValues[i];
  }

  float angle = atan2(sumY, sumX) * 180 / PI;
  if (angle < 0) {
    angle += 360;
  }

  float dist = sqrt(sumX * sumX + sumY * sumY) / sumValues;

  if (dist < analogRead(2)) {
    digitalWrite(1, HIGH);
  } else {
    digitalWrite(1, LOW);
  }

  angle = int(angle + 360) % 360;

  analogWrite(0, angle * 5 / 3.3);
  Serial.println(angle);
}

