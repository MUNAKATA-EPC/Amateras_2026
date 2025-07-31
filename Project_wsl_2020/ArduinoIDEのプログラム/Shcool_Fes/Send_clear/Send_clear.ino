int stick_x, stick_y, red_button, stick_button, judge_button;

void setup() {
  //シリアル通信設定
  Serial.begin(115200);   //センサーモニター開始
  Serial1.begin(115200);  //TWELITE通信開始

  //使うセンサーピンの設定
  pinMode(1, INPUT);  //stick_x入力
  pinMode(0, INPUT);  //stick_y入力
  pinMode(5, INPUT);  //stick_button入力
  pinMode(2, INPUT);  //red_button入力
  pinMode(3, INPUT);  //judge_button入力
}

void loop() {
  /*
  //各センサーの値を表示
  int xx = analogRead(0) - 515;
  Serial.print("XX : ");
  Serial.print(xx);
  int yy = analogRead(1) - 535;
  Serial.print("  YY : ");
  Serial.print(yy);
  Serial.print("  HUE : ");
  int hue = (180.0 * atan2(yy, xx)) / 3.14;
  if (xx > -20 && xx < 20 && yy > -20 && yy < 20) hue = 400;
  if (hue < 0) {
    hue = 360 + hue;
  }
  Serial.print(hue);

  Serial1.write(hue / 2);
  */

  if (digitalRead(3) == 1) {
    Serial1.write("c");
    Serial.println("c");
  } else {
    Serial1.write("d");
    Serial.println("d");
  }

  /*
  //TWELITEにメッセージを送信
  stick_x = analogRead(0);
  stick_y = analogRead(1);
  if (stick_x < 500) {
    if (stick_y < 500) {
      Serial1.write(1);
    } else if (stick_y > 600) {
      Serial1.write(2);
    } else {
      Serial1.write(3);
    }
  } else if (stick_x > 600) {
    if (stick_y < 500) {
      Serial1.write(4);
    } else if (stick_y > 600) {
      Serial1.write(5);
    } else {
      Serial1.write(6);
    }
  } else {
    Serial1.write(7);
  }

  //stick_button
  if (stick_button < 200) {
    Serial1.write(8);
  } else {
    Serial1.write(9);
  }
  //red_button
  if (red_button == LOW) {
    Serial1.write(10);
  } else {
    Serial1.write(11);
  }

  if (judge_button < 500) {
    Serial1.write(12);
  } else {
    Serial1.write(13);
  }
  */

  delay(10);
}
