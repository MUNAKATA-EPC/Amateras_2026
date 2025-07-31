float move_x = 0, move_y = 0, line_edge = 1023;
int line_total = 0;
int recoad_total = 0;
int lineCheck;

const int line_value = 600;
const int muxPin = 5;                     // マルチプレクサの出力ピン
const int controlPin[] = { 1, 2, 3, 4 };  // S0, S1, S2, S3 ピン
const int numSensors = 16;                // センサーの数
const int lineCheckNum = 20;

void setup() {
  pinMode(0, OUTPUT);
  analogWriteResolution(10);
  Serial.begin(9600);
  for (int i = 0; i < 4; i++) pinMode(controlPin[i], OUTPUT);  // コントロールピンを出力モードに設定
  pinMode(muxPin, INPUT);                                      // マルチプレクサの出力を入力モードに設定
}

void loop() {
  move_x = 0;
  move_y = 0;
  line_total = 0;
  recoad_total = 0;

  /*
  Serial.print("{ ");
  for (int i = 0; i < recoad_number; i++) {
    Serial.print(line_recoad[i]);
    recoad_total += line_recoad[i];
  }
  Serial.print(" }");
  */

  for (int i = 0; i <= 15; i++) {
    if (Mul_Read(i) < line_value) {
      move_x += cos((22.5 * i) * PI / 180);
      move_y += sin((22.5 * i) * PI / 180);

      line_total += 1;
      //Serial.print("1 ,");
    } else {
      //line_total += 0;
      //Serial.print("0 ,");
    }
  }

  /*
  if (line_total > 0) {
    line_recoad[line_count % recoad_number] = 1;
  } else {
    line_recoad[line_count % recoad_number] = 0;
  }
  */

  if (line_total != 0) {
    line_edge = 180 / PI * atan2(move_y, move_x) + 360;
    line_edge = int(line_edge) % 360;
    lineCheck = 0;
    for (int ii = 0; ii < 10; ii++) {
      for (int i = 0; i <= 15; i++) {
        if (Mul_Read(i) < line_value) {
          move_x += cos((22.5 * i) * PI / 180);
          move_y += sin((22.5 * i) * PI / 180);
          lineCheck += 1;
        }
      }
    }
    analogWrite(0, int(line_edge * 5 / 3.3));
    delay(500);
  } else {
    line_edge = -1;
    analogWrite(0, int(line_edge * 5 / 3.3));
  }

  Serial.print("Line_edge : ");
  Serial.println(line_edge);
  delay(100);
}

int Mul_Read(int Mul_number) {
  for (int i = 0; i <= 3; i++) {
    digitalWrite(controlPin[i], bitRead(Mul_number, i));
  }
  delay(1);
  return (analogRead(muxPin));
}
