//ライブラリを取り込む
#include <Sprite.h>
#include <Matrix.h>

Matrix myMatrix = Matrix(11, 13, 1);

//スプライトの指定
//幅、高さ、絵柄を二進数で指定
Sprite pattern1 = Sprite(
  8, 8,
  B01000010,
  B00100100,
  B00111100,
  B01011010,
  B11111111,
  B11111111,
  B10100101,
  B01000010
);


Sprite pattern2 = Sprite(
  8, 8,
  B01000010,
  B00100100,
  B10111101,
  B11011011,
  B11111111,
  B01111110,
  B00100100,
  B00011000
);

Sprite pattern3 = Sprite(
  8, 8,
  B01000010,
  B00100100,
  B01111110,
  B01011010,
  B11111111,
  B10100101,
  B01000010,
  B00000000
);


Sprite pattern4 = Sprite(
  8, 8,
  B00001111,
  B00001111,
  B00001111,
  B00001111,
  B11110000,
  B11110000,
  B11110000,
  B11110000
);


void setup()
{ 
}

//X座標の変数を用意
int x = 0;

void loop(){
  //スプライトの配置
  for(int i=0;i<16;i++){
  
  myMatrix.write(i-8, 0, pattern1);
  //点灯時間設定
  delay(200);
  //画面をクリア
  myMatrix.clear();

  myMatrix.write(i-8,  0, pattern2);
  delay(200);
  myMatrix.clear();
  }

}
