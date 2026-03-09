#include <RPlib.h>        //ロボプロシールドを使うためのライブラリ(設定ファイル)
#include <Matrix.h>
#include <Sprite.h>
#include  "SpriteData.h"
#include <Tone.h>
#include <Servo.h>

Matrix my_matrix = Matrix(11, 13, 1);
Tone my_tone;
Servo left_hand;
Servo right_hand;

void setup(){
  my_matrix.clear();                     //マトリクスLEDの表示を消す
  my_matrix.setBrightness(1);            // マトリクスLEDの明るさを設定(0−8)

  my_tone.begin(D3);                     // スピーカーがD3に接続されていることを設定

  left_hand.attach(S3); //servoLFと名づけたモーターをS0に接続
  right_hand.attach(S4); //servoRFと名づけたモーターをS1に接続

  const int a = 100;
  const int b = 500;

  for(int i = 0;i < a;i ++){
    if(i % 2 == 0){
      my_matrix.write(1, 2, big_heart); // 大きいハート
      my_tone.play(NOTE_C7, 100);
    }
    else{
      my_matrix.write(1, 2, small_heart); // 小さいハート
      my_tone.play(NOTE_C5, 100);
    }
    
    left_hand.write(i % 180);
    right_hand.write(i % 180);

    delay((i-a)*(i-a)*b/(a*a));

    my_matrix.clear();
  } 

  for(int i = 0;i < 15;i ++){
    int num = random(8);

    switch (num){
    case 0:
      my_matrix.write(1, 2, broken_heart1); // 壊れたハート
      my_tone.play(NOTE_C4, 100);
      break;
    case 1:
      my_matrix.write(1, 2, broken_heart2); // 壊れたハート
      my_tone.play(NOTE_C5, 100);
      break;
    case 2:
      my_matrix.write(1, 2, broken_heart3); // 壊れたハート
      my_tone.play(NOTE_E4, 100);
      break;
    case 3:
      my_matrix.write(1, 2, broken_heart4); // 壊れたハート
      my_tone.play(NOTE_E5, 100);
      break;
    case 4:
      my_matrix.write(1, 2, broken_heart1); // 壊れたハート
      my_tone.play(NOTE_F4, 100);
      break;
    case 5:
      my_matrix.write(1, 2, broken_heart2); // 壊れたハート
      my_tone.play(NOTE_F5, 100);
      break;
    case 6:
      my_matrix.write(1, 2, broken_heart3); // 壊れたハート
      my_tone.play(NOTE_G4, 100);
      break;
    case 7:
      my_matrix.write(1, 2, broken_heart4); // 壊れたハート
      my_tone.play(NOTE_G5, 100);
      break;
    }
    
    left_hand.write(num*90/8);
    right_hand.write(num*90/8);

    for(int ii = 5;ii < 5;ii ++){
      int x = random(8);
      int y = random(8);

      my_matrix.write(x, y, noise);
    }

    delay(100);

    my_matrix.clear();
  }

  delay(500);

  my_tone.play(NOTE_C5, 10000);
}

void loop(){
}
