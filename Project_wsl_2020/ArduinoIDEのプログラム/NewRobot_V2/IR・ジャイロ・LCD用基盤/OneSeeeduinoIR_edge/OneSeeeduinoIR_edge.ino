const int numSensors = 8;
const int sensorPins[numSensors] = { A7, A8, A9, A10, A3, A4, A5, A6 };
int sensorValues[numSensors];
float angles[numSensors];
float x[numSensors];
float y[numSensors];
float R = 52.50;  // センサーの円の半径（適宜変更してください）

float sumX = 0;
float sumY = 0;
float sumValues = 0;

float dist;
int dist_pin;

void setup() {
  pinMode(0, OUTPUT);
  analogWriteResolution(10);
  Serial.begin(9600);
  for (int i = 0; i < numSensors; i++) {
    pinMode(sensorPins[i], INPUT);
    angles[i] = i * (360.0 / numSensors);  // 各センサーの角度を計算
  }
  pinMode(6, OUTPUT);
}

void loop() {
  for (int i = 0; i < numSensors; i++) {
    sensorValues[i] = analogRead(sensorPins[i]);
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

  // IRボールまでの距離を求める
  dist = 1000;
  Serial.print(sensorValues[0]);
  Serial.print("/");
  for (int i = 0; i < numSensors; i++) {
    if (sensorValues[i] < dist) { dist = sensorValues[i]; }
    Serial.print(sensorValues[i]);
    Serial.print("/");
  }
  if(dist == 1000)angle = 600;

  Serial.print(" ==> ");
  Serial.print(angle);
  Serial.print(" , ");
  Serial.print(dist);
  Serial.print(" (");
  Serial.print(analogRead(2));
  Serial.print(") , ");


  if (dist < analogRead(2)) {
    analogWrite(1, 1023);
    Serial.println(1);
    analogWrite(0, angle * 5 / 3.3);
  } else {
    analogWrite(1, 0);
    Serial.println(0);
    analogWrite(0, angle * 5 / 3.3);
  }
}