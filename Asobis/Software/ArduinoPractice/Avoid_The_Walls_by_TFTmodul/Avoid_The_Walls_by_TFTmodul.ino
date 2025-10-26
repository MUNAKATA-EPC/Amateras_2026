#include <TFT.h>
#include <SPI.h>
#include <RPlib.h>

TFT TFTscreen = TFT(A2, 1, 9);        //液晶ディスプレイを使うときのオマジナイ

// 可変抵抗のピン
const int setting_adjuster_pin = A0;
const int char_y_adjuster_pin = A1;

// タクトスイッチ・LEDピン
const int tact_pin = A4;
const int led_pin = A5;

// キャラクターのスキン(可変)・当たり判定
char char_skin[2] = "D"; //97a~122z
const int char_area[2] = {
  5,8};

// 長方形同士の幅・高さ
const int rect_width = 10;
int rect_x = 160;
int rect_height = random(8,120);
// 長方形のスペース・速度格納用
int rect_space = 10;
int rect_speed = 2;

// 整数プリント用
void tftPrint(int data,int x,int y){
  String data_str = String(data);

  char data_char[10];                 //char配列準備
  data_str.toCharArray(data_char,10); //変換

  TFTscreen.text(data_char, x, y); //(x,y)にdataをプリント
}

void setup(){
  TFTscreen.begin();
  TFTscreen.background(0, 0, 0); // 黒でリセット

  pinMode(tact_pin, INPUT_PULLUP);    //タクトスイッチのピン設定
  pinMode(led_pin, OUTPUT);           //LEDのピン設定

  TFTscreen.stroke(100,255,0);      //緑で描画
  TFTscreen.setTextSize(2);         //文字サイズ2
  TFTscreen.text("- Avoid",0,0);
  TFTscreen.text("  The Walls -",6,2*8);

  TFTscreen.stroke(255,255,255);    //白で描画
  TFTscreen.setTextSize(1);         //文字サイズ1
  TFTscreen.text("speed:",10,8*3+8);
  TFTscreen.text("skin :",10,8*3+8+8);

  int select_mode = 0;
  int is_tact_on = 0,is_old_tact_on = 0;
  int setting_adjuster_val = 0,old_setting_adjuster_val = analogRead(setting_adjuster_pin); // 読み取っておく
  int setting_adjuster_moved = 0;
  while(1){
    //　タクトスイッチ読み取り
    is_tact_on = digitalRead(tact_pin) == LOW;
    // タクトスイッチによる計算
    if(is_tact_on && !is_old_tact_on){
      setting_adjuster_moved = 0;
      select_mode ++;
    }

    // 3回ボタンが押されたらスタート(break;でループ脱出)
    if(select_mode >= 2)break;

    // 可変抵抗の読み取り(setting_adjuster)
    setting_adjuster_val = analogRead(setting_adjuster_pin);
    // 可変抵抗が動かされたかどうか(タクトスイッチが押されるとリセット)
    if(abs(old_setting_adjuster_val - setting_adjuster_val) > 5)setting_adjuster_moved = 1;

    //　バー・長方形のスピード・スキンの計算
    int bar_x = 5,bar_y1,bar_y2;
    switch(select_mode){
    case 0: // スピード
      bar_y1=8*3+8;
      bar_y2=8*3+8+8;
      if(setting_adjuster_moved == 1)rect_speed = constrain((int)round(map(setting_adjuster_val,0,1023,1,10)),1,10);
      break;
    case 1: // スキン
      bar_y1=8*3+8+8;
      bar_y2=8*3+8+8+8;
      if(setting_adjuster_moved == 1)char_skin[0] = constrain((int)round(map(setting_adjuster_val,0,1023,'A','Z')),'A','Z');
      break;
    }

    // 描画
    TFTscreen.stroke(255,255,255);             // 白で描画
    TFTscreen.line(bar_x,bar_y1,bar_x,bar_y2); // バーをプリント
    tftPrint(rect_speed,10+8*6,8*3+8);         // 長方形のスピードをプリント
    TFTscreen.text(char_skin,10+8*6,8*3+8+8);  // スキンをプリント

    delay(10);

    // 消す描画
    TFTscreen.stroke(0,0,0);                   // 黒で描画
    TFTscreen.line(bar_x,bar_y1,bar_x,bar_y2); // バーを消す
    tftPrint(rect_speed,10+8*6,8*3+8);         // 長方形のスピードを消す
    TFTscreen.text(char_skin,10+8*6,8*3+8+8);  // スキンをプリントを消す

    //可変抵抗記録
    old_setting_adjuster_val = setting_adjuster_val;

    //タクトスイッチ記録
    is_old_tact_on = is_tact_on;
  }

  TFTscreen.stroke(255,255,255);             // 白のラインで描画
  tftPrint(rect_speed,10+8*6,8*3+8);         // 長方形のスピードを再プリント
  TFTscreen.text(char_skin,10+8*6,8*3+8+8);  // スキンを再プリント
  TFTscreen.text("->Game start!",10,8*3+8+8+8+8);
  delay(1000);

  TFTscreen.background(0, 0, 0); // 黒で再リセット
}

void loop(){
  //　長方形の描画
  TFTscreen.stroke(0,255,0);      // 緑のラインで描画
  TFTscreen.fill(0,255,0);        //緑の塗りつぶしを設定
  TFTscreen.rect(rect_x, 0, rect_width, rect_height);
  TFTscreen.rect(rect_x, rect_height + rect_space, rect_width, 128 - (rect_height + rect_space));

  //　キャラクターの座標計算・描画
  int char_x = 10;
  int char_y = map(analogRead(char_y_adjuster_pin),0,1023,0,128 - char_area[1]);

  TFTscreen.stroke(255,255,255); // 白で描画
  TFTscreen.text(char_skin,char_x,char_y);

  // 当たり判定計算
  int is_char_hit;
  if(char_x + char_area[0] < rect_x || rect_x + rect_width < char_x){
    // 長方形の外にいる
    is_char_hit = 0;
  }
  else{
    if(char_y >= rect_height && char_y + char_area[1] <= rect_height + rect_width){
      is_char_hit = 0;
    }
    else{
      is_char_hit = 1;
    }
  }

  //　当たってしまったらGameOverアニメーション
  if(is_char_hit){
    TFTscreen.background(0, 0, 0);//背景色にする（リセット）

    TFTscreen.setTextSize(2);         //文字サイズ2  
    TFTscreen.stroke(0,0,255); //赤で描画
    TFTscreen.text("Game",0,0);
    delay(1000);
    TFTscreen.stroke(255,0,0); //青で描画
    TFTscreen.text("Is",64,0);
    delay(1000);
    TFTscreen.stroke(0,255,0); //緑で描画
    TFTscreen.text("Over",96+8,0);
    delay(1000);

    while(1); // 終了
  }

  delay(10);

  // 消す描画
  TFTscreen.stroke(0,0,0); // 黒で描画
  TFTscreen.fill(0,0,0);   // 黒の塗りつぶしを設定
  TFTscreen.text(char_skin,char_x,char_y); // キャラクターを消す
  TFTscreen.rect(rect_x, 0, rect_width, rect_height); // 上の長方形を消す
  TFTscreen.rect(rect_x, rect_height + rect_space, rect_width, 128 - (rect_height + rect_space)); // 下の長方形を消す

  // 長方形移動・スペース位置の計算
  rect_x -= rect_speed;
  if(rect_x < 0){
    rect_x = 160;
    rect_height = random(0+8,128-8);
  }
}
























































