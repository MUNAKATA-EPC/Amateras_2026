#ifndef SSD1306_H
#define SSD1306_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "timer.hpp"

// I2C定義・開始
void SSD1306_init(TwoWire *wire, uint8_t adress, uint8_t width, uint8_t height);

// 表示をすべて消す
void SSD1306_clear();

// 表示させる
void SSD1306_show();

// 文字をプリントさせる
void SSD1306_write(uint8_t size, uint8_t ahead_x, uint8_t ahead_y, const char *Sentence, bool black);

// ビットマップを表示する
void SSD1306_bitmap(uint8_t x, uint8_t y, const uint8_t *bitmap, uint8_t width, uint8_t height, bool black);

// ラインを引く
void SSD1306_line(uint8_t start_x, uint8_t start_y, uint8_t end_x, uint8_t end_y, bool black);

// 直線y=ax+bを引く（y軸は上向きを正としている）
void SSD1306_function_line(uint8_t origin_x, uint8_t origin_y, double a, double b, bool black);

// 円を描画する
void SSD1306_circle(uint8_t point_x, uint8_t point_y, uint8_t circle_r, bool fill, bool black);

// 矩形を描画する
void SSD1306_rect(uint8_t topleft_x, uint8_t topleft_y, uint8_t width, uint8_t height, uint8_t corner_r, bool fill, bool black);

// 三角形を描画する
void SSD1306_triangle(uint8_t point1_x, uint8_t point1_y, uint8_t point2_x, uint8_t point2_y, uint8_t point3_x, uint8_t point3_y, bool fill, bool black);

#endif