#ifndef MOTORS_CONST_HPP
#define MOTORS_CONST_HPP

#include <Arduino.h>
#include "Motors.hpp"

// それぞれのチャンネルに接続したモータの角度を指定
void motors_init(int16_t deg_1ch, int16_t deg_2ch, int16_t deg_3ch, int16_t deg_4ch);

// 指定した方向に動かす（PD制御付き）
void motors_move(int16_t deg, int16_t abs_power);

/*ここのファイル内だけで使う関数*/

// degはロボット全体として進みたい方向
// powerはその方向へ進むときの合力の大きさ
void compute_motor_power(int16_t deg, int16_t power);

// index番目のモータの合力の大きさを返す
float get_motor_power(int8_t index);

// index番目のモータの出力をx方向に分解した値を返す
float get_motor_x_power(int8_t index);

// index番目のモータの出力をy方向に分解した値を返す
float get_motor_y_power(int8_t index);

#endif