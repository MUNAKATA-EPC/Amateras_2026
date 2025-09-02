#ifndef SSD1306_H
#define SSD1306_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "timer.hpp"

// I2C定義・開始
void SSD1306_init(TwoWire *wire, int adress, int width, int height);

// 表示をすべて消す
void SSD1306_clear();

// 表示させる
void SSD1306_show();

// 文字をプリントさせる
void SSD1306_write(int size, int ahead_x, int ahead_y, const char *Sentence, bool black);

// ビットマップを表示する
void SSD1306_bitmap(int x, int y, const int *bitmap, int width, int height, bool black);

// ラインを引く
void SSD1306_line(int start_x, int start_y, int end_x, int end_y, bool black);

// 直線y=ax+bを引く（y軸は上向きを正としている）
void SSD1306_function_line(int origin_x, int origin_y, double a, double b, bool black);

// 円を描画する
void SSD1306_circle(int point_x, int point_y, int circle_r, bool fill, bool black);

// 矩形を描画する
void SSD1306_rect(int topleft_x, int topleft_y, int width, int height, int corner_r, bool fill, bool black);

// 三角形を描画する
void SSD1306_triangle(int point1_x, int point1_y, int point2_x, int point2_y, int point3_x, int point3_y, bool fill, bool black);

#endif