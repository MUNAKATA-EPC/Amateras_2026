//-----------------------------------------------------------------------
//  タッチセンサーを検出して表示するプログラム
//  D2/3に接続してください
//-----------------------------------------------------------------------
// Copyright (C) Future Robotics Technology Center All Right Reserved
//-----------------------------------------------------------------------

#include <TFT.h>
#include <SPI.h>
#include <RPlib.h>

TFT TFTscreen = TFT(A2, 1, 9);                           //液晶ディスプレイを使うときのオマジナイ

void setup(){
  pinMode(D2, INPUT_PULLUP);                           //D2にタッチセンサーを接続
  pinMode(D3, INPUT_PULLUP);                           //D3にタッチセンサーを接続
  TFTscreen.begin();
  TFTscreen.background(0, 0, 0);                       //背景色は黒
  TFTscreen.stroke(255, 255, 255);                     //線の色は白
}

int oldNum = 0;

void loop(){
  TFTscreen.stroke(255, 255, 255);                     //線の色は白

  int displayClear = 0;

  int isD2On = digitalRead(D2);
  int isD3On = digitalRead(D3);

  if(isD2On){
    if(isD3On){
      TFTscreen.text("Double Pushing", 0, 0);                 //(0,0)にテキストを表示する

      if(oldNum != 0) displayClear = 1;
      oldNum = 0;
    }
    else{
      TFTscreen.text("D2 Pushing", 0, 0);                 //(0,0)にテキストを表示する

      if(oldNum != 1) displayClear = 1;
      oldNum = 1;
    }
  }
  else if(isD3On){
    TFTscreen.text("D3 Pushing", 0, 0);                 //(0,0)にテキストを表示する

    if(oldNum != 2) displayClear = 1;
    oldNum = 2;
  }
  else{
    TFTscreen.text("None", 0, 0);                 //(0,0)にテキストを表示する

    if(oldNum != 3) displayClear = 1;
    oldNum = 3;
  }

  if(displayClear == 1)TFTscreen.background(0, 0, 0);                       //背景色は黒
}







