//ライブラリを取り込む
#include <Sprite.h>
#include <Matrix.h>
#include <RPlib.h>
#include <Tone.h>
Matrix myMatrix = Matrix(11, 13, 1);      //マトリクスLEDを使うときのオマジナイ
Tone tone1;
char *sound1 = "M1A:d=4,o=5,b=120:16g,16c6,16g5,16c6,16g5,16c6";

Sprite minute_character = Sprite(
5, 5,
B11011,
B11011,
B11011,
B10101,
B10101
);
Sprite second_character = Sprite(
5, 5,
B01110,
B01000,
B01110,
B00010,
B01110
);

int double_click = 0;     //０：押されてない、１：押された

int setting_number[2] = {
  0,0                                          };   //０番最終決定用、１番選択途中

int setting_minute = 0;
int setting_second = 0;

int matrix_xx = 0;
int matrix_yy = 0;

void setup(void){
  pinMode(10, INPUT_PULLUP);   //D0
  pinMode(9, INPUT_PULLUP);   //D1

  myMatrix.setBrightness(3);    //マトリクスLEDの明るさを設定(0−8)
  tone1.begin(D2);              //D2にスピーカーを接続
}

void loop(void){
  setting_number[0] = 0;
  //時間設定
  while(setting_number[0] != 2){
    //決定ボタンが押されるまでループ
    setting_number[0] = setting_number[0];
    setting_number[1] = setting_number[0];   //０：minute、１：second、２：決定
    double_click = 0;        //押されてない
    while(double_click == 0){
      //時間設定中
      if(digitalRead(9) == HIGH){ //D0スイッチが入ったら
        delay(10);
        if(digitalRead(10) == HIGH){ //D1スイッチが入ったら
          double_click = 1;
          //どっちも押されたのでこの設定で決定
          setting_number[0] = abs(setting_number[1] % 3);
        }
        else{
          double_click = 0;
          //D0のみ押された
          setting_number[1] ++;
        }
      }
      else{
        if(digitalRead(10) == HIGH){ //D1スイッチが入ったら
          double_click = 0;
          //D1のみ押された
          setting_number[1] --;
        }
        else{
          double_click = 0;
          //押されてないので何もしない
        }
      }

      //表示
      if(abs(setting_number[1] % 3) == 0){
        myMatrix.putch(0, 0, 'M');
      }
      else if(abs(setting_number[1] % 3) == 1){
        myMatrix.putch(0, 0, 'S');
      }
      else if(abs(setting_number[1] % 3) == 2){
        myMatrix.putch(0, 0, 'D');
      }

      delay(150);
      myMatrix.clear();
    }

    if(setting_number[0] == 0){//minute設定
      double_click = 0;
      while(double_click == 0){
        if(digitalRead(9) == HIGH){ //D0スイッチが入ったら
          delay(10);
          if(digitalRead(10) == HIGH){ //D1スイッチが入ったら
            double_click = 1;
            //どっちも押されたのでこの設定で決定
          }
          else{
            double_click = 0;
            //D0のみ押された
            setting_minute ++;
            if(setting_minute > 59)setting_minute = 59;
          }
        }
        else{
          if(digitalRead(10) == HIGH){ //D1スイッチが入ったら
            double_click = 0;
            //D1のみ押された
            setting_minute --;
            if(setting_minute < 0)setting_minute = 0;
          }
          else{
            double_click = 0;
            //押されてないので何もしない
          }
        }
        myMatrix.putd2(0,0,setting_minute);
        delay(100);
        myMatrix.clear();
      }
    }
    else if(setting_number[0] == 1){//second設定
      double_click = 0;
      while(double_click == 0){
        if(digitalRead(9) == HIGH){ //D0スイッチが入ったら
          delay(10);
          if(digitalRead(10) == HIGH){ //D1スイッチが入ったら
            double_click = 1;
            //どっちも押されたのでこの設定で決定
          }
          else{
            double_click = 0;
            //D0のみ押された
            setting_second ++;
            if(setting_second > 59)setting_second = 59;
          }
        }
        else{
          if(digitalRead(10) == HIGH){ //D1スイッチが入ったら
            double_click = 0;
            //D1のみ押された
            setting_second --;
            if(setting_second < 0)setting_second = 0;
          }
          else{
            double_click = 0;
            //押されてないので何もしない
          }
        }
        myMatrix.putd2(0,0,setting_second);
        delay(100);
        myMatrix.clear();
      }
    }
  }

  double_click = 0;
  while(double_click == 0){
    if(digitalRead(9) == HIGH){ //D0スイッチが入ったら
      delay(10);
      if(digitalRead(10) == HIGH){ //D1スイッチが入ったら
        double_click = 1;
        //どっちも押されたのでこの設定で決定
        myMatrix.clear();
        delay(500);
      }
      else{
        double_click = 0;
        //D0のみ押された
        matrix_yy = -3;

        matrix_yy = constrain(matrix_yy,-3,0);
        myMatrix.putd2(0,matrix_yy,setting_minute);
        myMatrix.putd2(1,matrix_yy + 6,setting_second);
        delay(100);
      }
    }
    else{
      if(digitalRead(10) == HIGH){ //D1スイッチが入ったら
        double_click = 0;
        //D1のみ押された
        matrix_yy = 0;

        myMatrix.putd2(0,matrix_yy,setting_minute);
        myMatrix.putd2(1,matrix_yy + 6,setting_second);
        delay(100);
      }
      else{
        double_click = 0;
        //押されてないので何もしない
        myMatrix.putd2(0,matrix_yy,setting_minute);
        myMatrix.putd2(1,matrix_yy + 6,setting_second);
        delay(100);
      }
    }
    myMatrix.clear();
  }

  int zero_judge = 0;
  while(zero_judge == 0){
    if (setting_second == 0) {
      if (setting_minute == 0) {
        zero_judge = 1;
      } 
      else {
        zero_judge = 0;

        setting_minute --; // 分を減らす
        setting_second = 59; // 秒を59にリセット
      }
    } 
    else {
      zero_judge = 0;

      setting_second --; // 秒を減らす
    }

    if(zero_judge == 0){
      for(int i = 0; i < 50; i++){
        if(digitalRead(10) == HIGH){
          matrix_yy = -3;
        }
        if(digitalRead(9) == HIGH){
          matrix_yy = 0;
        }
        myMatrix.putd2(0, matrix_yy, setting_minute);
        myMatrix.putd2(1, matrix_yy + 6, setting_second);
        delay(20); // 1ミリ秒待つ
        myMatrix.clear();
      }
    }



    /*
    myMatrix.putd2(0,matrix_yy,setting_minute);
     myMatrix.putd2(1,matrix_yy + 6,setting_second);
     delay(1000);
     */
  }
  tone1.play_rtttl(sound1);
  delay(10);
  myMatrix.clear();
}


































