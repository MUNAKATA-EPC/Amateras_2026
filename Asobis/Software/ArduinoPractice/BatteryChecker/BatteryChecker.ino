//-----------------------------------------------------------------------
// 7セグメントLED（ボリュームセンサー）
//-----------------------------------------------------------------------
// Copyright (C) Future Robotics Technology Center All Right Reserved
//-----------------------------------------------------------------------

#include <LedControl.h>
#define AVECOUNT 25

LedControl lc = LedControl(11, 13, 1); //7セグメントLEDを使うときのオマジナイ

void LEDmeter(int minPin,int maxPin,int Value,int maxValue);

void setup(){
  lc.shutdown(0, false);             //7セグメントLEDをリセット
  lc.setIntensity(0, 4);             //色の濃さ(0-8)
  lc.clearDisplay(0);                //表示クリア
}

void loop(){
  long AveV = 0;
  for(int i = 0;i < AVECOUNT; i++){
    int V = map(analogRead(A0),0,1023,0,5000);
    AveV += V;
    delay(2);
  }
  AveV /= AVECOUNT; 
  lc.setDec(0,AveV);
  lc.setLed(0,0,7,HIGH);
}




