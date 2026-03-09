//-----------------------------------------------------------------------
// LEDグラフ表示④
// ウエーブ
//-----------------------------------------------------------------------
// Copyright (C) Future Robotics Technology Center All Right Reserved
//-----------------------------------------------------------------------

//ライブラリを取り込む
#include <Sprite.h>
#include <Matrix.h>

Matrix myMatrix = Matrix(11, 13, 1);    //マトリクスLEDを使うときのオマジナイ

void setup(){
  myMatrix.clear();                   //マトリクスLEDの表示を消す
}

void loop(){
  for(int i = -7;i <= 7;i ++){
    Graph_I(1,i);
    Graph_I(1,-i);
    Graph_I(-1,7+i);
    Graph_I(-1,-i+7);
    delay(100);
    myMatrix.clear();               //マトリクスLEDの表示を消す
  }  
  
  for(int i = 0;i <= 7;i ++){
    dot_Y(i);
    delay(8);

    myMatrix.clear();           //マトリクスLEDの表示を消す
  }
  for(int i = 6;i >= 1;i --){
    dot_X(i);
    delay(8);

    myMatrix.clear();           //マトリクスLEDの表示を消す
  }
}

void Graph_I(float a,float b){
  int x , y;
  for(x = 0; x <= 7; x++){
    y = a * x + b;                  //一次関数(切片i)
    myMatrix.write(x,7 - y, HIGH); //LEDを点灯
  }
}

void dot_Y(int y1){
  myMatrix.line(0,y1,7,7-y1);
}

void dot_X(int x1){
  myMatrix.line(x1,0,7-x1,7);
}






