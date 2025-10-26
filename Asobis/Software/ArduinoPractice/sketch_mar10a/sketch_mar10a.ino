#include <RPlib.h>
#include <PS2X_lib.h>
#include <Sprite.h>
#include <Matrix.h>
#include <Tone.h>
#include <ColorSensor.h>

//おまじない(ピン接続設定)
RPmotor mc1(MC1);
RPmotor mc2(MC2);
PS2X ps2x;
Matrix myMatrix = Matrix(11, 13, 1);  //マトリクスLEDを使うときのオマジナイ
Tone tone1;          //スピーカーを使うときのオマジナイ

int rx, lx;
double button = 1.0;
int vibrate = 0;
int matrix = 1;
int button2 = 1;
int button3 = 1;
int dist1 , dist2;
byte red(0), green(0), blue(0);

void setup(){
  ps2x.config_gamepad(13, 11, 10, 12, true, true); //コントローラーを使うときのオマジナイ
  myMatrix.setBrightness(3);        //マトリクスLEDの明るさを設定(0−8)
  tone1.begin(D3); //D3にスピーカーを接続
  ColorSensor.begin(100);                       //カラーセンサーの積分時間を指定 x10[ms]　 値が大きいほどセンサー値の精度が良くなるが、そのぶん時間がかかる　100で10[ms] 最大 6144
}

void loop(){
  ps2x.read_gamepad();                             //コントローラーから値を読み取る
  ps2x.read_gamepad(false, vibrate);               //何ボタンが押されたかな？

  if(ps2x.Button(PSB_L1)){
    button = 1.0;
    tone1.play(523,300); //523Hzの音を出す(C
  }
  if(ps2x.Button(PSB_R1)){
    button = 1.8;
    tone1.play(523,300); //587Hzの音を出す(C
  }
  if(button == 1.0){
    matrix = 1;
    myMatrix.line(0, 5, 7 , 5, LOW);
  }
  else{
    matrix = 2;
    myMatrix.line(0, 5, 7 , 5, HIGH);
  }
  myMatrix.putd2(0, 0, matrix);        //マトリクスLEDに2けたの数字を表示

  rx = ps2x.Analog(PSS_RY);                        //アナログ右スティック上下
  lx = ps2x.Analog(PSS_LY);                        //アナログ左スティック上下
  dist1 = ussRead(US1);
  dist2 = ussRead(US2);
  if(ps2x.Button(PSB_R3)){
    tone1.play(659,300);     //ミ
    button3 = 1;
  }
  if(ps2x.Button(PSB_L3)){
    tone1.play(659,300);     //ミ
    button3 = 0;
  }
  if(button3 == 1){
    myMatrix.line(0, 7, 7 , 7, HIGH);        //マトリクスLEDに2けたの数字を表示
  }
  else{
    myMatrix.line(0, 7, 7 , 7, LOW);        //マトリクスLEDに2けたの数字を表示
  }
  if(button3 == 1){
    if(dist1 > 15 || dist2 > 15){
      tone1.play(659,300);     //ミ
      rx = -rx;
      lx = -lx;
      mc1.rotate(-rx * button);                                 //右モーターを回す
      mc2.rotate(lx * button);                                  //左モーターを回す
      delay(100);
    }
  }
  mc1.rotate(-rx * button);                                 //右モーターを回す
  mc2.rotate(lx * button);                                  //左モーターを回す

  if(ps2x.Button(PSB_PAD_UP)){
    mc1.rotate(-200);                                 //右モーターを回す
    mc2.rotate(200);                                  //左モーターを回す
    delay(100);
    mc1.rotate(0);                                 //右モーターを回す
    mc2.rotate(0);                                  //左モーターを回す
    delay(100);      
  }
  if(ps2x.Button(PSB_PAD_DOWN)){
    mc1.rotate(200);                                 //右モーターを回す
    mc2.rotate(-200);                                //左モーターを回す 
    delay(100);  
    mc1.rotate(0);                                 //右モーターを回す
    mc2.rotate(0);                                  //左モーターを回す
    delay(100);      
  }
  if(ps2x.Button(PSB_PAD_LEFT)){
    mc1.rotate(-200);                                 //右モーターを回す
    mc2.rotate(-200);                                  //左モーターを回す 
    delay(100);
    mc1.rotate(0);                                 //右モーターを回す
    mc2.rotate(0);                                  //左モーターを回す
    delay(100);         
  }
  if(ps2x.Button(PSB_PAD_RIGHT)){
    mc1.rotate(200);                                 //右モーターを回す
    mc2.rotate(200);                                  //左モーターを回す
    delay(100);
    mc1.rotate(0);                                 //右モーターを回す
    mc2.rotate(0);                                  //左モーターを回す
    delay(100);         
  }

  if(ps2x.Button(PSB_L2)){
    button2 = 1;
    tone1.play(587,300); //587Hzの音を出す(レ
  }
  if(ps2x.Button(PSB_R2)){
    button2 = 2;
    tone1.play(587,300); //587Hzの音を出す(レ
  }
  if(button2 == 1){
    myMatrix.line(0, 6, 7 , 6, LOW);        //マトリクスLEDに2けたの数字を表示
    red = 0;
    green = 0;
    blue = 0;
  }
  else{
    myMatrix.line(0, 6, 7 , 6, HIGH);        //マトリクスLEDに2けたの数字を表示
    if(ps2x.Button(PSB_RED)){
      red = red + 10;
      green = 0;
      blue = 0;
    }
    else if(ps2x.Button(PSB_GREEN)){
      green = green + 10;
      red = 0;
      blue = 0;
    }
    else if(ps2x.Button(PSB_BLUE)){
      blue = blue + 10;
      red = 0;
      green = 0;
    }
    else if(ps2x.Button(PSB_PINK)){
      red = 100;
      green = 100;
      blue = 100;
    }
  }
  if(red > 100)red = 100;
  if(blue > 100)blue = 100;
  if(green > 100)green = 100;

  ColorSensor.ledRGB(red, green, blue);

  myMatrix.clear();                //マトリクスLEDの表示を消す
  delay(10);
}


