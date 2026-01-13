#include <Sprite.h>
#include <Matrix.h>
Matrix myMatrix = Matrix(11, 13, 1);  //マトリクスLEDを使うときのオマジナイ

#include <PS2X_lib.h>
PS2X ps2x;

#include <RPlib.h>
RPmotor mc0(MC0);
RPmotor mc1(MC1);
RPmotor mc2(MC2);

int control_rx , control_ry;
float hue(0.0);
float rx , ry;

void setup(){
  myMatrix.setBrightness(5);        //マトリクスLEDの明るさを設定(0−8)
  ps2x.config_gamepad(13,11,10,12, true, true); //コントローラーを使うときのオマジナイ
}

void loop(){
  //コントローラーの角度を計算
  ps2x.read_gamepad();
  rx = (float)ps2x.Analog(PSS_RX);
  ry = (float)ps2x.Analog(PSS_RY);
  hue = atan2(ry, rx);
  if(hue < 0){
    hue = hue + 2 * PI;
  }
  control_rx = rx / 42;
  control_ry = ry / 42;

  //ロボットの動き
  if(abs(rx) > abs(ry)){
    right_con(int (abs(1.85 * rx)) );
  }
  else{
    right_con(int (abs(1.85 * ry)) );
  }

  delay(10);
}

//functions

void right_con(int speeed){
  myMatrix.clear(); //画面を消す
  myMatrix.line(4 - control_rx,3 + control_ry,3 - control_rx,3 + control_ry,HIGH); //画面にラインを引く
  myMatrix.line(4 - control_rx,4 + control_ry,3 - control_rx,4 + control_ry,HIGH); //画面にラインを引く

  myMatrix.line(1,1,6,1,HIGH); //画面にラインを引く
  myMatrix.line(1,6,6,6,HIGH); //画面にラインを引く
  myMatrix.line(6,5,6,2,HIGH); //画面にラインを引く
  myMatrix.line(1,5,1,2,HIGH); //画面にラインを引く

  if(control_rx == 0  &&  control_ry == 0){
    mc1.rotate(0);        //100の速度で正方向に回す(範囲は-255から255)
    mc2.rotate(0);        //100の速度で正方向に回す(範囲は-255から255)
    mc0.rotate(0);        //100の速度で正方向に回す(範囲は-255から255)
  }
  else{
    mc2.rotate(speeed * sin(hue));        //100の速度で正方向に回す(範囲は-255から255)
    mc0.rotate(speeed * sin(hue + 2 * PI / 3));        //100の速度で正方向に回す(範囲は-255から255)
    mc1.rotate(speeed * sin(hue + 4 * PI / 3));        //100の速度で正方向に回す(範囲は-255から255)
  }

  if(ps2x.Button(PSB_R1)){
    mc1.rotate(50);        //100の速度で正方向に回す(範囲は-255から255)
    mc2.rotate(50);        //100の速度で正方向に回す(範囲は-255から255)
    mc0.rotate(50);        //100の速度で正方向に回す(範囲は-255から255)
  }
  else if(ps2x.Button(PSB_L1)){
    mc1.rotate(-50);        //100の速度で正方向に回す(範囲は-255から255)
    mc2.rotate(-50);        //100の速度で正方向に回す(範囲は-255から255)
    mc0.rotate(-50);        //100の速度で正方向に回す(範囲は-255から255)
  }
  else if(ps2x.Button(PSB_R2)){
    mc1.rotate(100);        //100の速度で正方向に回す(範囲は-255から255)
    mc2.rotate(100);        //100の速度で正方向に回す(範囲は-255から255)
    mc0.rotate(100);        //100の速度で正方向に回す(範囲は-255から255)
  }
  else if(ps2x.Button(PSB_L2)){
    mc1.rotate(-100);        //100の速度で正方向に回す(範囲は-255から255)
    mc2.rotate(-100);        //100の速度で正方向に回す(範囲は-255から255)
    mc0.rotate(-100);        //100の速度で正方向に回す(範囲は-255から255)
  }
}



























