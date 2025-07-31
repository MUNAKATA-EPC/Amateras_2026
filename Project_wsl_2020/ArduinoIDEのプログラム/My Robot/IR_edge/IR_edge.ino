const int numSensors = 8;
const int sensorPins[numSensors] = { A7, A8, A9, A10, A3, A4, A5, A6 };
int sensorValues[numSensors];
int sensorMin[3][2];
float angles[numSensors];
float x[numSensors];
float y[numSensors];
float R = 1000;  // センサーの円の半径（適宜変更してください）

float sumX = 0;
float sumY = 0;
float sumValues = 0;

float dist;

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
  sensorMin[1][0] = -1;
  sensorMin[1][1] = 1023;
  for (int i = 0; i < numSensors; i++) {
    sensorValues[i] = analogRead(sensorPins[i]);
    x[i] = R * cos(radians(angles[i]));
    y[i] = R * sin(radians(angles[i]));
    if (sensorValues[i] < sensorMin[1][1]) {
      sensorMin[1][0] = i;
      sensorMin[1][1] = sensorValues[i];
    }
  }
  sensorMin[0][0] = (sensorMin[1][0] + 7) % 8;
  sensorMin[2][0] = (sensorMin[1][0] + 1) % 8;
  sensorMin[0][1] = sensorValues[sensorMin[0][0]];
  sensorMin[2][1] = sensorValues[sensorMin[2][0]];

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

  /*// IRボールまでの距離を求める
  dist = abs(sqrt(sumX / 1000 * sumX / 1000 + sumY / 1000 * sumY / 1000));*/
  dist = (sensorMin[0][1] + sensorMin[1][1] + sensorMin[2][1]) / 3;

  if (dist < analogRead(2)) {
    digitalWrite(1, HIGH);
  } else {
    digitalWrite(1, LOW);
  }

  analogWrite(0, int((angle + 135) * 5 / 3.3));

  Serial.print("IR_edge : ");
  Serial.print(angle);
  //Serial.print(sumValues);
  Serial.print("  ,  ");
  Serial.print("IR_position : ( ");
  Serial.print(sumX / 1000);
  Serial.print(" , ");
  Serial.print(sumY / 1000);
  Serial.print(" )  ,  ");
  Serial.print("DAC : ");
  Serial.print(int(angle * 5 / 3.3));
  Serial.print(",  ");
  Serial.print("dist : ");
  Serial.print(dist);
  Serial.print(",  ");
  Serial.print("posi : ");
  Serial.print(analogRead(2));
  Serial.println("");
  delay(10);
}