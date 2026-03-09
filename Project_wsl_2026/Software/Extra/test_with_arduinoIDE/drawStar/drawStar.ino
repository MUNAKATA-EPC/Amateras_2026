//-----------------------------------------------------------------------
// 星を描く
//-----------------------------------------------------------------------
// Copyright (C) Future Robotics Technology Center All Right Reserved
//-----------------------------------------------------------------------
//ライブラリを読み込む
#include <RPlib.h>
#include <VarSpeedServo.h>
#include "ArmRobot.h"
#include <math.h>

ArmRobot armBot;                           //アームロボットを使うときのオマジナイ

void setup(){
  pinMode(D3, INPUT_PULLUP);             //D3にタッチセンサーを接続

  while (!digitalRead(D3));              //D3に接続されたタッチセンサーを読み取る
  //#######################################################################
  armBot.setup(-10,0,0,-10);  
  //#######################################################################
  delay(3000);

  while (!digitalRead(D3));              //D3に接続されたタッチセンサーを読み取る
  armBot.setServoSpeed(SERVO_R,   10);   //サーボモーターの速度調整値(1-255)
  armBot.setServoSpeed(SERVO_L,   10);   //サーボモーターの速度調整値(1-255)
  armBot.setPosition(100, 0, 0);         //アームの根元から100mmの位置
  armBot.setServoSpeed(SERVO_ROT,   10); //サーボモーターの速度調整値(1-255)
}

//ここから変更-------------------------------------------------

#define SIDE 30                            //星の大きさを指定
#define P_NUM 6                            //座標の数(変更するところ)
double start_point[1][2] = {
  0, 0
};

void loop(){
  while (!digitalRead(D3));     //D3に接続されたタッチセンサーを読み取る

  double point[P_NUM][2] = {    //移動座標(変更するところ)
    {
      0, SIDE
    }                         //Point 1
    , {
      SIDE * cos(-126 * PI / 180), SIDE * sin(-126 * PI / 180)
      }                         //Point 2
    , {
      SIDE * cos(18 * PI / 180), SIDE * sin(18 * PI / 180)
      }                         //Point 3
    , {
      SIDE * cos(162 * PI / 180), SIDE * sin(162 * PI / 180)
      }                         //Point 4
    , {
      SIDE * cos(-54 * PI / 180), SIDE * sin(-54 * PI / 180)
      }                         //Point 5
    , {
      0, SIDE
    }                         //Point 6
  };

  int pen_status[P_NUM - 1] = { //ペンの状態(変更するところ)
    DOWN,                     //Point1-2
    DOWN,                     //Point2-3
    DOWN,                     //Point3-4
    DOWN,                     //Point4-5
    DOWN                      //Point5-6
  };

  armBot.penUp();
  armBot.traceLine(start_point[0], point[0]);            //図形の初期位置へ移動
  delay(1000);

  //ペンの状態を制御する
  for (int i = 0; i < P_NUM - 1; i++){
    if (pen_status[i] == UP){
      armBot.penUp();
      delay(1000);
    }
    if (pen_status[i] == DOWN){
      armBot.penDown();
      delay(1000);
    }

    armBot.traceLine(point[i], point[i + 1]);          //ラインを描く
    delay(2000);
  }
  armBot.penUp();
  delay(1000);
  armBot.traceLine(point[P_NUM - 1], start_point[0]);    //初期位置へ移動
}

