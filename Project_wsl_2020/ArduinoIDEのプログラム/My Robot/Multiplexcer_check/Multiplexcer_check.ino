const int muxPin = 5;                     // マルチプレクサの出力ピン
const int controlPin[] = { 1, 2, 3, 4 };  // S0, S1, S2, S3 ピン
const int numSensors = 16;                // センサーの数

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 4; i++) pinMode(controlPin[i], OUTPUT);  // コントロールピンを出力モードに設定
  pinMode(muxPin, INPUT);                                      // マルチプレクサの出力を入力モードに設定
}

void loop() {
  for (int i = 0; i <= 15; i++) {
    Serial.print(Mul_Read(i));
    Serial.print("  ,  ");
  }
  Serial.println("");
  delay(100);
}

int Mul_Read(int Mul_number) {
  for (int i = 0; i <= 3; i++) {
    digitalWrite(controlPin[i], bitRead(Mul_number, i));
  }
  delay(1);  // 安定化のための遅延
  return (analogRead(muxPin));
}
