#ifndef SSD1306_H
#define SSD1306_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "timer.hpp"

// I2C定義・開始
void SSD1306_init(TwoWire *wire, uint8_t adress, int width, int height);

// 表示をすべて消す
void SSD1306_clear();

// 表示させる
void SSD1306_show();

// 文字をプリントさせる
void SSD1306_write(int size, double ahead_x, double ahead_y, String Sentence, bool black);

// ビットマップを表示する
void SSD1306_bitmap(double x, double y, const unsigned char *bitmap, int width, int height, bool black);

// ラインを引く
void SSD1306_line(double start_x, double start_y, double end_x, double end_y, bool black);

// 直線y=ax+bを引く（y軸は上向きを正としている）
void SSD1306_function_line(double origin_x, double origin_y, double a, double b, bool black);

//  円を描画する
void SSD1306_circle(double point_x, double point_y, double circle_r, bool fill, bool black);

// 矩形を描画する
void SSD1306_rect(double topleft_x, double topleft_y, double width, double height, double corner_r, bool fill, bool black);

// 三角形を描画する
void SSD1306_triangle(double point1_x, double point1_y, double point2_x, double point2_y, double point3_x, double point3_y, bool fill, bool black);

#endif