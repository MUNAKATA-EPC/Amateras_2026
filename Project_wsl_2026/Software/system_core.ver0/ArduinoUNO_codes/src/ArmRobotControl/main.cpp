//-----------------------------------------------------------------------
// コントローラーでアームロボットを動かす
//-----------------------------------------------------------------------
// Copyright (C) Future Robotics Technology Center All Right Reserved
//-----------------------------------------------------------------------
//ライブラリを読み込む
#include <Arduino.h>
#include <RPlib.h>
#include <VarSpeedServo.h>
#include <PS2X_lib.h>

#define ARM_LONG 128 //アームの長さ

#define ARM1_MINANGLE 20
#define ARM1_MAXANGLE 110
#define ARM2_MINANGLE 20
#define ARM2_MAXANGLE 150

VarSpeedServo ServoArm2;    //サーボモーターを使うときのオマジナイ(servoRと名づける)
VarSpeedServo ServoArm1;    //サーボモーターを使うときのオマジナイ(servoLと名づける)
VarSpeedServo ServoTurn;  //サーボモーターを使うときのオマジナイ(servoROTと名づける)
VarSpeedServo ServoCatch; //サーボモーターを使うときのオマジナイ(servoGRIPと名づける)

PS2X ps2x;               //コントローラーに名前をつける

void setup(){
  pinMode(D3,INPUT_PULLUP);
  ps2x.config_gamepad(13, 11, 10, 12, true, true);       //コントローラーを使うときのオマジナイ
  Serial.begin(9600);                                    //シリアル通信を使うときのオマジナイ

  ServoArm2.attach(S0, 0, 180);   //servoRと名づけたモーターをS0に接続
  ServoArm1.attach(S1, 0, 180);   //servoLと名づけたモーターをS1に接続
  ServoTurn.attach(S2, 0, 180); //servoROTと名づけたモーターをS2に接続
  ServoCatch.attach(S3, 0, 180);  //servoGRIPと名づけたモーターをS3に接続
}

void loop(){
  ps2x.read_gamepad();        //コントローラーから値を読み取る

  int ps2_ly = ps2x.Analog(PSS_LY);  //アナログ左スティック上下
  //int ps2_ry = ps2x.Analog(PSS_RY);  //アナログ右スティック上下
  //int ps2_rx = ps2x.Analog(PSS_RX);  //アナログ右スティック左右
  //int ps2_lx = ps2x.Analog(PSS_LX);  //アナログ左スティック左右

  double ArmX = ps2_ly;
  double ArmY = sqrt(ARM_LONG * ARM_LONG + ArmX * ArmX / 4);
  double Arm1Angle = atan2(ArmY,ArmX) * 180 / PI;
  Arm1Angle = constrain(Arm1Angle,ARM1_MINANGLE,ARM1_MAXANGLE);
  double Arm2Angle = Arm1Angle;
  Arm2Angle = constrain(Arm2Angle,ARM2_MINANGLE,ARM2_MAXANGLE);

  ServoArm1.write(Arm1Angle);
  ServoArm2.write(Arm2Angle);

  delay(100);
}
