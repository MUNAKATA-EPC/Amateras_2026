//-----------------------------------------------------------------------
// タクトスイッチを検出する
//-----------------------------------------------------------------------
// Copyright (C) Future Robotics Technology Center All Right Reserved
//-----------------------------------------------------------------------

#include <TFT.h>
#include <SPI.h>
#include <RPlib.h>

TFT TFTscreen = TFT(A2, 1, 9);           //液晶ディスプレイを使うときのオマジナイ

const int tactPins[4] = {
  A0,A1,A4,A5};

void setup(){
  for(int i = 0;i < 4;i ++){
    pinMode(tactPins[i], INPUT_PULLUP);           //A0にタクトスイッチを接続
  }

  TFTscreen.begin();
  TFTscreen.background(0, 0, 0);       //背景色は黒
}

int oldTactBitMask = 0x0000;

void loop(){
  TFTscreen.stroke(255, 255, 255);     //線の色は白

  int isTactOn[4];
  int tactBitMask = 0x0000;
  char text[4];
  for(int i = 0;i < 4;i ++){
    isTactOn[i] = digitalRead(tactPins[i]) == LOW ? 0x0001 : 0x0000;

    tactBitMask |= (isTactOn[i] << i);

    text[i] = isTactOn[i] ? '1' : '0';
  }

  TFTscreen.text(text, 0, 0);  //描画

  if(oldTactBitMask != tactBitMask)
    TFTscreen.background(0, 0, 0);                       //状況が変わればリセット

  oldTactBitMask = tactBitMask;
}





