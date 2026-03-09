//ライブラリを取り込む
#include <Sprite.h>
#include <Matrix.h>

Matrix myMatrix = Matrix(11, 13, 1);

//スプライトの指定
//幅、高さ、絵柄を二進数で指定
Sprite pattern1 = Sprite(
  8, 8,
  B11111111,
  B00000000,
  B11111111,
  B00000000,
  B11111111,
  B00000000,
  B11111111,
  B00000000
);

Sprite pattern2 = Sprite(
  8, 8,
  B10101010,
  B10101010,
  B10101010,
  B10101010,
  B10101010,
  B10101010,
  B10101010,
  B10101010
);

Sprite pattern3 = Sprite(
  8, 8,
  B11110000,
  B11110000,
  B11110000,
  B11110000,
  B00001111,
  B00001111,
  B00001111,
  B00001111
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
  myMatrix.write(0, 0, pattern1);
  //点灯時間設定
  delay(200);
  //画面をクリア
  myMatrix.clear();

  myMatrix.write(0,  0, pattern2);
  delay(200);
  myMatrix.clear();

  myMatrix.write(0, 0, pattern3);
  delay(200);
  myMatrix.clear();

  myMatrix.write(0, 0, pattern4);
  delay(200);
  myMatrix.clear();
}
