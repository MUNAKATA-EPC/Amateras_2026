#include <TFT.h>
#include <SPI.h>

TFT display = TFT(A2, 1, 9);

void setup(){
  display.begin();
  display.background(0, 0, 0);
  display.stroke(255, 255, 255);
}

//色見本データ
const char R[] = {
  0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0xc0
};
const char G[] = {
  0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0xc0
};
const char B[] = {
  0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x80, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0xc0
};

char* get_clear_chara(){
  char clear_chara[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  };
  return clear_chara;
}

void charaPrint(int x0,int y0,char* data,int data_size_x,int data_size_y,int scale_width,int scale_height){
  for (int y = 0; y < data_size_y; y++){
    for (int x = 0; x < data_size_x; x++){
      char colorIndex = data[y * data_size_x + x];
      display.stroke(B[colorIndex], G[colorIndex], R[colorIndex]);
      display.fill(B[colorIndex], G[colorIndex], R[colorIndex]);
      display.rect(x0 + x * scale_width, y0 + y * scale_height, scale_width, scale_height);
    }
  }
}

long int count = 0;

void loop(){
  int c = random(1,8);
  char chara[] = {
    0, 0, 0, 0, 0, c, c, c, c, 0, 0, 0, 0, 0,
    0, 0, 0, c, c, c, c, c, c, c, c, 0, 0, 0,
    0, 0, c, c, c, c, c, c, c, c, c, c, 0, 0,
    0, c, 7, 7, c, c, c, c, 7, 7, c, c, c, 0,
    0, 7, 7, 7, 7, c, c, 7, 7, 7, 7, c, c, 0,
    0, 1, 1, 7, 7, c, c, 1, 1, 7, 7, c, c, 0,
    c, 1, 1, 7, 7, c, c, 1, 1, 7, 7, c, c, c,
    c, c, 7, 7, 0, 0, c, c, 7, 7, c, c, c, c,
    c, c, c, 0, 7, c, c, c, c, c, c, c, c, c,
    0, c, c, 0, 7, c, c, c, c, c, c, c, c, 0,
    c, c, c, c, 0, 0, 0, c, c, c, c, c, c, c,
    c, c, c, c, c, c, c, c, c, c, c, c, c, c,
    c, c, c, c, 0, c, c, c, c, 0, c, c, c, c,
    0, c, c, 0, 0, 0, c, c, 0, 0, 0, c, c, 0
  };

  int shake_x = random(-10,10);
  int shake_y = random(-10,10);

  charaPrint(160 / 2 - 14 / 2 * 3 + shake_x, 128 / 2 - 14 / 2 * 3 + shake_y, chara, 14, 14, 3, 3);

  int r1 = 45;
  int x1 = round(r1 * cos(radians( count * 6 )) + 160 / 2);
  int y1 = round(r1 * sin(radians( count * 6 )) + 128 / 2);
  charaPrint(x1 + shake_x,y1 + shake_y,chara,14,14,1,1);

  int r2 = 55;
  int x2 = round(r2 * cos(-radians( count * 12 )) + 160 / 2);
  int y2 = round(r2 * sin(-radians( count * 12 )) + 128 / 2);
  charaPrint(x2 + shake_x,y2 + shake_y,chara,14,14,1,1);

  delay(10);
  
  charaPrint(160 / 2 - 14 / 2 * 3 + shake_x, 128 / 2 - 14 / 2 * 3 + shake_y, get_clear_chara(), 14, 14, 3, 3);
  charaPrint(x1 + shake_x,y1 + shake_y,get_clear_chara(),14,14,1,1);
  charaPrint(x2 + shake_x,y2 + shake_y,get_clear_chara(),14,14,1,1);

  count ++;
}





