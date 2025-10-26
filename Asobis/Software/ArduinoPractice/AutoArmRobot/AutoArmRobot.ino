//-----------------------------------------------------------------------
// 自動搬送②(対象物へ向かう)
//-----------------------------------------------------------------------
// Copyright (C) Future Robotics Technology Center All Right Reserved
//-----------------------------------------------------------------------
#include <RPlib.h>
#include <VarSpeedServo.h>
#include <ArmRobot.h>
#include <Ultrasonic.h>

Ultrasonic ultrasonic(US1);                            //超音波距離センサーのインスタンスを生成
ArmRobot armBot;                                       //アームロボットを使うときのオマジナイ

void setup(){
  Serial.begin(9600);                                //シリアル通信を使うときのオマジナイ
  pinMode(D3, INPUT_PULLUP);                         //スタートボタンの設定
  /*while(!Serial.available());
  while(Serial.available())Serial.read();*
  Serial.println("press to start");
  while(!digitalRead(D3));
  /***********************************/
  armBot.setup(0.0, 0.0, 0.0, -7);                  //調整値(servoR,servoL,servoROT,servoGRIPの順)
  /***********************************/
  armBot.setServoSpeed(SERVO_R,   10);               //0=full speed, 1-255 slower to faster
  armBot.setServoSpeed(SERVO_L,   10);               //0=full speed, 1-255 slower to faster
  armBot.setPosition(150, 50, 0);
  armBot.setServoSpeed(SERVO_R,   20);               //0=full speed, 1-255 slower to faster
  armBot.setServoSpeed(SERVO_L,   20);               //0=full speed, 1-255 slower to faster
  armBot.setServoSpeed(SERVO_ROT,   20);             //0=full speed, 1-255 slower to faster
}
float mm,old_mm = 0,gap_of_mm = 0;
void loop(){
  long microsec = ultrasonic.timing();               //センサー情報を計測
  mm = ultrasonic.convert(microsec, Ultrasonic::MM); //センサー情報を変換

  //距離をシリアルで表示
  Serial.print("dis: ");
  Serial.print(mm);
  Serial.print("[mm]\t");
  Serial.print("gap: ");
  Serial.print(gap_of_mm);
  Serial.println("[mm]");

  int Height = 100;
  int Width = mm;
  int Turn = -10;
  gap_of_mm = abs(mm - old_mm);
  if(mm < 155 && gap_of_mm < 20){
    Serial.println("find");
    armBot.setPosition(Width, Height, Turn);                     //対象との距離によって動く
    delay(1000);
    Serial.println("set");
    while(Height > 6){
      armBot.setPosition(Width,Height,Turn);
      Serial.println("down...");
      Height--;
      delay(10);
    }
    Serial.println("set");
    delay(1000);
    armBot.gripperCatch();
    Serial.println("catch");
    delay(1000);
    while(Height < 100){
      armBot.setPosition(Width,Height,Turn);
      Serial.println("up...");
      Height++;
      delay(10);
    }
    Serial.println("set");
    delay(1000);
    while(Turn < 40){
      armBot.setPosition(Width,Height,Turn);
      Serial.println("turn...");
      Turn++;
      delay(10);
    }
    Serial.println("set");
    delay(1000);
    armBot.gripperRelease();
    Serial.println("release");
  }
  else{
    armBot.setPosition(50, Height, Turn); 
    armBot.gripperRelease();
  }
  old_mm = mm;
}






