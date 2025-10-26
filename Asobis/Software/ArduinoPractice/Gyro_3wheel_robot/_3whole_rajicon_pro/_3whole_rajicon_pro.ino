//-----------------------------------------------------------------------
// オムニホイールロボットをコントローラーで操縦する
// 調整用プログラム
//-----------------------------------------------------------------------
// Copyright (C) Future Robotics Technology Center All Right Reserved
//-----------------------------------------------------------------------
#include <RPomniDirect.h>
#include <RPlib.h>
#include <PS2X_lib.h>
#include <Sprite.h>
#include <Matrix.h>


//おまじない
RPomniDirect omniBot(1.0f, 1.0f, 0.9f, 10.0f);       //オムニホイール調整用パラメータ
PS2X ps2x;
Matrix myMatrix = Matrix(11, 13, 1);  //マトリクスLEDを使うときのオマジナイ

float xx, yy;                                        //移動速度変数
int void_x , void_y , void_z , select = 3 , power = 1;

void setup(){
  ps2x.config_gamepad(13, 11, 10, 12, true, true); //おまじない（ピン設定)
  set_up();
}

void loop(){
  
  if(ps2x.Button(PSB_L3))set_up();
  if(select == 0){
    void_x = sq_x(100);
    void_y = sq_y(100);
    void_z = sq_z(64);
  }
  else{
    void_x = cr_x();
    void_y = cr_y();
    void_z = cr_z();
  }
  omniBot.move( void_x , void_y , void_z );
  power ++;
  if(power > 8)power = 1;
  board(power);
  delay(100);
  myMatrix.clear();
}


//↓関数↓


//最初のセットアップ
void set_up(){
  omniBot.move( 0 , 0 , 0 );
  ps2x.read_gamepad();
  while(ps2x.Button(PSB_L3)){
    ps2x.read_gamepad();
  }
  select = 3;
  myMatrix.clear();
  do{
    ps2x.read_gamepad(); 
    if(ps2x.Analog(PSS_LY) < -30){
      select = 1;
    }
    else if(ps2x.Analog(PSS_LY) > 30){
      select = 0;
    }

    if(select == 1){
      myMatrix.line(3,0,4,0,HIGH); //画面にラインを引く
      myMatrix.line(2,1,5,1,HIGH); //画面にラインを引く
      myMatrix.line(2,2,5,2,HIGH); //画面にラインを引く
      myMatrix.line(3,3,4,3,HIGH); //画面にラインを引く

      myMatrix.line(2,4,5,7,LOW); //画面にラインを引く
      myMatrix.line(2,7,5,4,LOW); //画面にラインを引く
    }
    else if(select == 0){
      myMatrix.line(2,4,5,7,HIGH); //画面にラインを引く
      myMatrix.line(2,7,5,4,HIGH); //画面にラインを引く

      myMatrix.line(3,0,4,0,LOW); //画面にラインを引く
      myMatrix.line(2,1,5,1,LOW); //画面にラインを引く
      myMatrix.line(2,2,5,2,LOW); //画面にラインを引く
      myMatrix.line(3,3,4,3,LOW); //画面にラインを引く
    }
    else{
      myMatrix.line(2,4,5,7,HIGH); //画面にラインを引く
      myMatrix.line(2,7,5,4,HIGH); //画面にラインを引く

      myMatrix.line(3,0,4,0,HIGH); //画面にラインを引く
      myMatrix.line(2,1,5,1,HIGH); //画面にラインを引く
      myMatrix.line(2,2,5,2,HIGH); //画面にラインを引く
      myMatrix.line(3,3,4,3,HIGH); //画面にラインを引く
    }

    delay(100);
  }
  while( !(ps2x.Button(PSB_L3)) || select == 3);

  myMatrix.clear();

  if(select == 1){
    myMatrix.line(1,3,6,3,HIGH); //画面にラインを引く
    myMatrix.line(1,4,6,4,HIGH); //画面にラインを引く
    myMatrix.line(2,2,5,2,HIGH); //画面にラインを引く
    myMatrix.line(2,5,5,5,HIGH); //画面にラインを引く
    myMatrix.line(3,1,4,1,HIGH); //画面にラインを引く
    myMatrix.line(3,6,4,6,HIGH); //画面にラインを引く
  }
  else{
    myMatrix.line(1,6,6,1,HIGH); //画面にラインを引く
    myMatrix.line(1,1,6,6,HIGH); //画面にラインを引く
  }
  ps2x.read_gamepad();
  while(ps2x.Button(PSB_L3)){
    ps2x.read_gamepad();
  }
}



//十字キーのxの設定
int sq_x(int x_speed){
  int x;
  ps2x.read_gamepad();                             //コントローラーから値を読み込む

  if(ps2x.Button(PSB_RED)){
    x = x_speed;
  }
  else if(ps2x.Button(PSB_PINK)){
    x = - x_speed;
  }
  else{
    x = 0;
  }
  return x;
}

//十字キーのyの設定
int sq_y(int y_speed){
  int y;
  if(ps2x.Button(PSB_GREEN)){
    y = y_speed;
  }
  else if(ps2x.Button(PSB_BLUE)){
    y = - y_speed;
  }
  else{
    y = 0;
  }
  return y;
}

//十字キーのzの設定
int sq_z(int z_speed){
  int z;
  if(ps2x.Button(PSB_R1)){
    z = z_speed;
  }
  else if(ps2x.Button(PSB_L1)){
    z = - z_speed;
  }
  else{
    z = 0;
  }
  return z;
}



//360スティックのxの設定
int cr_x(){
  ps2x.read_gamepad(); 
  double x = ps2x.Analog(PSS_RX) / 1.4;
  return int(x);
}

//360スティックのyの設定
int cr_y(){
  ps2x.read_gamepad(); 
  double y = ps2x.Analog(PSS_RY) / 1.4;
  return int(y);
}

//360スティックのzの設定
int cr_z(){
    ps2x.read_gamepad();
    float hue(0.0);

    float rx = (float)ps2x.Analog(PSS_LX);
    float ry = (float)ps2x.Analog(PSS_LY);
    
    hue = (180.0 * atan2(ry, rx) / 3.14) - 90.0;

    if(hue < 0)hue = 360.0 + hue;
    if(ps2x.Analog(PSS_LY) == 0 && ps2x.Analog(PSS_LX) == 0)hue = 0.0;
    if(hue > 180)hue = hue - 360.0;
    
    return(int(-hue / 1.8));
}



//Matrixの設定
void board(int light){
  myMatrix.setBrightness(light);        //マトリクスLEDの明るさを設定(0−8)
  if( void_x == -100 ){
    if( void_y == -100 ){
      myMatrix.line(6,1,7,0,HIGH); //画面にラインを引く
    }  //右前
    else if( void_y == 100 ){
      myMatrix.line(6,6,7,7,HIGH); //画面にラインを引く
    }  //右後
    else{
      myMatrix.line(7,3,7,4,HIGH); //画面にラインを引く
    }  //右
  }
  else if( void_x == 100 ){
    if( sq_y(100) == -100 ){
      myMatrix.line(1,1,0,0,HIGH); //画面にラインを引く
    }  //左前
    else if( void_y == 100 ){
      myMatrix.line(1,6,0,7,HIGH); //画面にラインを引く
    }  //左後
    else{
      myMatrix.line(0,3,0,4,HIGH); //画面にラインを引く
    }  //左
  }
  else{
    if( void_y == -100 ){
      myMatrix.line(3,0,4,0,HIGH); //画面にラインを引く
    }  //前
    else if( void_y == 100 ){
      myMatrix.line(3,7,4,7,HIGH); //画面にラインを引く
    }  //後
  }
  if( void_z == 64 && select == 0 || void_z > 0 && select == 1 ){
    myMatrix.line(5,5,2,5,HIGH); //画面にラインを引く
    myMatrix.line(2,4,2,2,HIGH); //画面にラインを引く
    myMatrix.write(1,3,HIGH); //画面にラインを引く
    myMatrix.write(3,3,HIGH); //画面にラインを引く
  }
  else if( void_z == -64 && select == 0 || void_z < 0 && select == 1 ){
    myMatrix.line(5,5,2,5,HIGH); //画面にラインを引く
    myMatrix.line(5,5,5,2,HIGH); //画面にラインを引く
    myMatrix.write(4,3,HIGH); //画面にラインを引く
    myMatrix.write(6,3,HIGH); //画面にラインを引く
  }
  else if(select == 0){
    myMatrix.line(2,5,5,2,HIGH); //画面にラインを引く
    myMatrix.line(2,2,5,5,HIGH); //画面にラインを引く
  }
  else{
    myMatrix.line(3,2,4,2,HIGH); //画面にラインを引く
    myMatrix.line(3,5,4,5,HIGH); //画面にラインを引く
    myMatrix.line(2,3,5,3,HIGH); //画面にラインを引く
    myMatrix.line(2,4,5,4,HIGH); //画面にラインを引く
  }
}




























