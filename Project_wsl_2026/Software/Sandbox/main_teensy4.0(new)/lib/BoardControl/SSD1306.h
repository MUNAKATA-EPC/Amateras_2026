#ifndef MYLIB_SSD1306_H

#define MYLIB_SSD1306_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// LINEIRSW Checkerのビットマップ
extern const unsigned char LINEIRSWChecker_bitmap[] PROGMEM;

// SSD1306を動かす関数
void SSD1306begin();
// SSD1306の表示をすべて消す関数
void SSD1306clear();
// SSD1306に表示させる関数
void SSD1306show();
// SSD1306に文字をプリントさせる関数
void SSD1306write(int size, double ahead_x, double ahead_y, String Sentence, bool black);
// SSD1306に文字をプリントさせる関数
void SSD1306line(String Color);
// SSD1306にビットマップを表示する関数
void SSD1306Bitmap(double x, double y, const unsigned char *bitmap, int width, int height, bool black);
// SSD1306にラインを引く関数
void SSD1306line(double start_x, double start_y, double end_x, double end_y, bool black);
// 直線y=ax+bを引く関数（y軸は上向きを正としている）
void SSD1306functionline(double origin_x, double origin_y, double a, double b, bool black);
//  SSD1306に円を描画する関数
void SSD1306Circle(double point_x, double point_y, double circle_r, bool fill, bool black);
// SSD1306に矩形を描画する関数
void SSD1306Rect(double topleft_x, double topleft_y, double width, double height, double corner_r, bool fill, bool black);
// SSD1306に三角形を描画する関数
void SSD1306Triangle(double point1_x, double point1_y, double point2_x, double point2_y, double point3_x, double point3_y, bool fill, bool black);

#endif