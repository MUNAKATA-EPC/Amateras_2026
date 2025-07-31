void setup() {
  // シリアル通信の開始
  Serial1.begin(19200);
}

void loop() {
  brake_moter('1');    // 1chを停止
  delay(1000);
  control_motor('1',-100);    // 1chは-100%で動かす
  delay(1000);
}


//関数一覧


// I2Cアドレス変更コマンド
void change_i2c_address(char n) {
  Serial1.print("A:");
  Serial1.print(n);
  Serial1.print("\r\n");
}

// シリアル通信速度コマンド
void change_serial_speed(char n) {
  Serial1.print("S:");
  Serial1.print(n);
  Serial1.print("\r\n");
}

// モータ制御コマンド
void control_motor(char ch, int speed) {
  char direction;
  if(speed < 0){
    direction = 'R';
    speed = -speed;
  }else{
    direction = 'F';
  }
  Serial1.print(ch);
  Serial1.print(direction);
  if (speed < 10) {
    Serial1.print("00");
  } else if (speed < 100) {
    Serial1.print("0");
  }
  Serial1.print(speed);
  Serial1.print("\r\n");
}

//ブレーキ
void brake_moter(char ch){
  Serial1.print(ch);
  Serial1.print('R');
  Serial1.print("000");
  Serial1.print("\r\n");
}