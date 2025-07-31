const int numSensors = 8;
const int sensorPins[numSensors] = { A5, A8, A9, A10, A1, A2, A3, A4 };
int sensorValues[numSensors];
float angles[numSensors];
float x[numSensors];
float y[numSensors];
float R = 52.50;  // センサーの円の半径（適宜変更してください）

float sumX = 0;
float sumY = 0;
float sumValues = 0;

float dist;

void setup() {
  pinMode(0, OUTPUT);
  analogWriteResolution(10);
  Serial.begin(9600);
  Serial1.begin(2000000);
  for (int i = 0; i < numSensors; i++) {
    pinMode(sensorPins[i], INPUT);
    angles[i] = i * (360.0 / numSensors);  // 各センサーの角度を計算
  }
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
  dist = 1000 - sqrt(sumX * sumX + sumY * sumY) * 100 / sumValues;
  // IRボールがあるのか否か
  if (sumValues > 8180) {
    angle = -1;
    dist = 1;
  }

  Serial.print("角度: ");
  Serial.print(angle);
  Serial1.print(int(angle));
  Serial1.print('\n');  // 改行を追加して送信
  analogWrite(0, angle * 5 / 3.3);
  delay(50);

  Serial.print("  距離: ");
  Serial.println(dist + 1000);
  if(angle != -1)Serial1.print(int(dist) + 1000);
  Serial1.print('\n');  // 改行を追加して送信
  delay(50);
}
