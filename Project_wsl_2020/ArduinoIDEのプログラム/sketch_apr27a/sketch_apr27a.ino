//-----------------------------------------------------------------------
// マイコンボードのポート9に繋がったLEDを点滅させる
//-----------------------------------------------------------------------
// Copyright (C) Future Robotics Technology Center All Right Reserved
//-----------------------------------------------------------------------
int led = 9;                 //ledという名の変数を用意する(led=9)

void setup(){
  pinMode(led, OUTPUT);    //9番のピンを出力に設定する(led=9)
}

void loop(){
  for(int i = 0;i <= 255;i ++){
    analogWrite(led,i);
    delay(100);
  }
  for(int i = 255;i >= 0;i --){
    analogWrite(led,i);
    delay(100);
  }
}
