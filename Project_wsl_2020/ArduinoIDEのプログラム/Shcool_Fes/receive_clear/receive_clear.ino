char receive;

void setup() {
  Serial1.begin(115200);
  Serial.begin(9600);
  pinMode(9, OUTPUT);
}

void loop() {
  if (Serial1.available()) {
    receive = Serial1.read();
    if (receive != 13 && receive != 10 && receive != 0) {
      if (receive == 'd') {
        Serial.println("judge_on");
        digitalWrite(9, HIGH);
      } else {
        Serial.println("off");
        digitalWrite(9, LOW);
      }
    }
  }

  delay(10);
}


//関数一覧
