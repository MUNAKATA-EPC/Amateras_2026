//-----------------------------------------------------------------------
// ラインの平行移動
// 平行ウエーブ
//-----------------------------------------------------------------------
// Copyright (C) Future Robotics Technology Center All Right Reserved
//-----------------------------------------------------------------------

//ライブラリを取り込む
#include <Sprite.h>
#include <Matrix.h>

Matrix myMatrix = Matrix(11, 13, 1); //マトリクスLEDを使うときのオマジナイ

void setup(){
  myMatrix.clear();                //マトリクスLEDの表示を消す
}

float turn_speed = 100;

void loop(){
  /*
  //home_work1
   
   turn_speed = 100;
   for(int a = 1;a <= 7;a ++){
   for(int i = 0;i <= 7;i ++){
   dot_Y(i);
   delay(turn_speed);
   
   turn_speed -= 1.020408163265306;
   
   myMatrix.clear();           //マトリクスLEDの表示を消す
   }
   for(int i = 6;i >= 1;i --){
   dot_X(i);
   delay(turn_speed);
   
   turn_speed -= 1.020408163265306;
   
   myMatrix.clear();           //マトリクスLEDの表示を消す
   }
   }
   for(int a = 1;a <= 5;a ++){
   for(int i = 0;i <= 7;i ++){
   dot_Y(i);
   delay(turn_speed);
   
   myMatrix.clear();           //マトリクスLEDの表示を消す
   }
   for(int i = 6;i >= 1;i --){
   dot_X(i);
   delay(turn_speed);
   
   myMatrix.clear();           //マトリクスLEDの表示を消す
   }
   }
   
   for(int a = 1;a <= 7;a ++){
   for(int i = 0;i <= 7;i ++){
   dot_Y(i);
   delay(turn_speed);
   
   turn_speed += 1.020408163265306;
   
   myMatrix.clear();           //マトリクスLEDの表示を消す
   }
   for(int i = 6;i >= 1;i --){
   dot_X(i);
   delay(turn_speed);
   
   turn_speed += 1.020408163265306;
   
   myMatrix.clear();           //マトリクスLEDの表示を消す
   }
   }
   for(int a = 1;a <= 5;a ++){
   for(int i = 0;i <= 7;i ++){
   dot_Y(i);
   delay(turn_speed);
   
   myMatrix.clear();           //マトリクスLEDの表示を消す
   }
   for(int i = 6;i >= 1;i --){
   dot_X(i);
   delay(turn_speed);
   
   myMatrix.clear();           //マトリクスLEDの表示を消す
   }
   }
   
   */

  /*
   //home_work2
   
   for(int i = 0;i <= 7;i ++){
   myMatrix.line(i,0,i,7);
   myMatrix.line(0,i,7,i);
   delay(turn_speed);
   
   myMatrix.clear();           //マトリクスLEDの表示を消す
   }
   
   */   
}

void dot_Y(int y1){
  myMatrix.line(0,y1,7,7-y1);
}

void dot_X(int x1){
  myMatrix.line(x1,0,7-x1,7);
}













