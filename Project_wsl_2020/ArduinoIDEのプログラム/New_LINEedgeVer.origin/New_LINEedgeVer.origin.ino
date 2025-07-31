const int muxPin = 5;                     // マルチプレクサの出力ピン
const int controlPin[] = { 1, 2, 3, 4 };  // S0, S1, S2, S3 ピン
const int numSensors = 16;                // センサーの数

void setup() {
  pinMode(0, OUTPUT);
  analogWriteResolution(10);
  Serial.begin(9600);
  for (int i = 0; i < 4; i++) pinMode(controlPin[i], OUTPUT);  // コントロールピンを出力モードに設定
  pinMode(muxPin, INPUT);                                      // マルチプレクサの出力を入力モードに設定
}

void loop() {
  int ThreeLineDATA[4] = {
    Mul_Read(0) + Mul_Read(1) + Mul_Read(2) + Mul_Read(3),
    Mul_Read(4) + Mul_Read(5) + Mul_Read(6) + Mul_Read(7),
    Mul_Read(8) + Mul_Read(9) + Mul_Read(10) + Mul_Read(11),
    Mul_Read(12) + Mul_Read(13) + Mul_Read(14) + Mul_Read(15),
  };

  analogWrite(0, -1);
  int LineEdge = -1;
  for (int i = 0; i < 4; i++) {
    if (ThreeLineDATA[i] > 0) {
      LineEdge = 45 * i + 45;
      analogWrite(0, LineEdge * 5 / 3.3);
      delay(500);
      analogWrite(0, -1);
    }
  }
  Serial.println(LineEdge);
  delay(10);
}

int Mul_Read(int Mul_number) {
  for (int i = 0; i <= 3; i++) {
    digitalWrite(controlPin[i], bitRead(Mul_number, i));
  }
  delay(1);
  return (abs(1 - digitalRead(muxPin)));
}
