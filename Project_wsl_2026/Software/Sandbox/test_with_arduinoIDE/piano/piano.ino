//-----------------------------------------------------------------------
// マトリクスLEDを使ったコントローラーの動作確認①
//-----------------------------------------------------------------------
// Copyright (C) Future Robotics Technology Center All Right Reserved
//-----------------------------------------------------------------------
//ライブラリを読み込む
#include <PS2X_lib.h>
#include <Sprite.h>
#include <Matrix.h>
#include <RPlib.h>
#include <Tone.h>

PS2X ps2x;                                           //コントローラーに名前をつける
Matrix myMatrix = Matrix(11, 13, 1);                 //マトリクスLEDを使うときのオマジナイ
Tone tone1;

int vibrate = 0;                                     //変数intをvibrateという名前で使う(はじめは0)

void setup(){
  ps2x.config_gamepad(13, 11, 10, 12, true, true); //コントローラーを使うときのオマジナイ
  myMatrix.clear();                                //マトリクスLEDの表示を消す
  myMatrix.setBrightness(3);                       //マトリクスLEDの明るさを設定(0−8)
}

void loop(){
  ps2x.read_gamepad(false, vibrate); //コントローラーから値を読み取る

  float L_hue(0.0);
  float lrx = (float)ps2x.Analog(PSS_LX);
  float lry = (float)ps2x.Analog(PSS_LY);
  L_hue = 180.0 * atan2(lry, lrx) / 3.14;
  L_hue -= 180;
  if(L_hue < 0)L_hue = 360 + L_hue; 
  if(L_hue < 180){
    L_hue /= 60;
  }
  else{
    L_hue -= 360;
    L_hue /= -60;
  }
  if(-2 <= lrx && lrx <= 2  &&  -2 <= lry && lry <= 2)L_hue = -4;
  L_hue += 4;

  float R_hue(0.0);
  float rrx = (float)ps2x.Analog(PSS_RX);
  float rry = (float)ps2x.Analog(PSS_RY);
  R_hue = 180.0 * atan2(rry, rrx) / 3.14;
  R_hue -= 51.5;
  if(R_hue < 0)R_hue = 360 + R_hue; 
  if(-2 <= rrx && rrx <= 2  &&  -2 <= rry && rry <= 2)R_hue = -1;

  if(R_hue == -1){
    myMatrix.clear();
  }
  else if(R_hue / 51.5 < 1){
    myMatrix.putch(0, 0, 'C');     //”C”を表示
    if(L_hue == 0){
      tone1.play(NOTE_C4, 10);
    }
    else if(L_hue == 4){
      tone1.play(NOTE_C4, 10);
    }
    else if(L_hue == 5){
      tone1.play(NOTE_C5, 10);
    }
    else if(L_hue == 6){
      tone1.play(NOTE_C6, 10);
    }
    else{
      tone1.play(NOTE_C7, 10);
    }
  }
  else if(R_hue / 51.5 < 2){
    myMatrix.putch(0, 0, 'D');
    if(L_hue == 0){
      tone1.play(NOTE_D4, 10);
    }
    else if(L_hue == 4){
      tone1.play(NOTE_D4, 10);
    }
    else if(L_hue == 5){
      tone1.play(NOTE_D5, 10);
    }
    else if(L_hue == 6){
      tone1.play(NOTE_D6, 10);
    }
    else{
      tone1.play(NOTE_D7, 10);
    }
  }
  else if(R_hue / 51.5 < 3){
    myMatrix.putch(0, 0, 'E');
    if(L_hue == 0){
      tone1.play(NOTE_E4, 10);
    }
    else if(L_hue == 4){
      tone1.play(NOTE_E4, 10);
    }
    else if(L_hue == 5){
      tone1.play(NOTE_E5, 10);
    }
    else if(L_hue == 6){
      tone1.play(NOTE_E6, 10);
    }
    else{
      tone1.play(NOTE_E7, 10);
    }
  }
  else if(R_hue / 51.5 < 4){
    myMatrix.putch(0, 0, 'F');
    if(L_hue == 0){
      tone1.play(NOTE_F4, 10);
    }
    else if(L_hue == 4){
      tone1.play(NOTE_F4, 10);
    }
    else if(L_hue == 5){
      tone1.play(NOTE_F5, 10);
    }
    else if(L_hue == 6){
      tone1.play(NOTE_F6, 10);
    }
    else{
      tone1.play(NOTE_F7, 10);
    }
  }
  else if(R_hue / 51.5 < 5){
    myMatrix.putch(0, 0, 'G');
    if(L_hue == 0){
      tone1.play(NOTE_G4, 10);
    }
    else if(L_hue == 4){
      tone1.play(NOTE_G4, 10);
    }
    else if(L_hue == 5){
      tone1.play(NOTE_G5, 10);
    }
    else if(L_hue == 6){
      tone1.play(NOTE_G6, 10);
    }
    else{
      tone1.play(NOTE_G7, 10);
    }
  }
  else if(R_hue / 51.5 < 6){
    myMatrix.putch(0, 0, 'A');
    if(L_hue == 0){
      tone1.play(NOTE_A4, 10);
    }
    else if(L_hue == 4){
      tone1.play(NOTE_A4, 10);
    }
    else if(L_hue == 5){
      tone1.play(NOTE_A5, 10);
    }
    else if(L_hue == 6){
      tone1.play(NOTE_A6, 10);
    }
    else{
      tone1.play(NOTE_A7, 10);
    }
  }
  else{
    myMatrix.putch(0, 0, 'B');
    if(L_hue == 0){
      tone1.play(NOTE_B4, 10);
    }
    else if(L_hue == 4){
      tone1.play(NOTE_B4, 10);
    }
    else if(L_hue == 5){
      tone1.play(NOTE_B5, 10);
    }
    else if(L_hue == 6){
      tone1.play(NOTE_B6, 10);
    }
    else{
      tone1.play(NOTE_B7, 10);
    }
  }

  if( -2 <= rrx && rrx <= 2  &&  -2 <= rry && rry <= 2){
    myMatrix.clear();
  }
  else{
    if(L_hue == 0 && -2 <= rrx && rrx <= 2  &&  -2 <= rry && rry <= 2){
      myMatrix.line(0,7,7,7,LOW);
    }
    else if(L_hue == 0){
      myMatrix.line(0,7,1,7,HIGH);
    }
    else{
      int mat = 2 * (L_hue - 4);
      myMatrix.line(mat + 1,7,7,7,LOW);
      myMatrix.line(0,7,mat,7,HIGH);
    }
  }
  delay(50);
}




















