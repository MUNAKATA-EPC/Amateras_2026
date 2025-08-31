//インンベーダーが動く

//ライブラリを取り込む
#include <Sprite.h>
#include <Matrix.h>

Matrix myMatrix = Matrix(11, 13, 1);

//スプライトの指定
//幅、高さ、絵柄を二進数で指定
Sprite pattern1 = Sprite(
  8, 8,
  B00011000,
  B00111100,
  B01111110,
  B11011011,
  B11111111,
  B00100100,
  B01011010,
  B10100101
);


Sprite pattern2 = Sprite(
  8, 8,
  B00011000,
  B00111100,
  B01111110,
  B11011011,
  B11111111,
  B00100100,
  B01100110,
  B01010010
);

void setup()
{ 
}

//X座標の変数を用意
int x = 0;

void loop(){
  //スプライトの配置
  for(x=0;x<16;x++){
  
  myMatrix.write(x-8, 0, pattern1);
  //点灯時間設定
  delay(200);
  //画面をクリア
  myMatrix.clear();

  myMatrix.write(x-8,  0, pattern2);
  delay(200);
  myMatrix.clear();
  }

}
