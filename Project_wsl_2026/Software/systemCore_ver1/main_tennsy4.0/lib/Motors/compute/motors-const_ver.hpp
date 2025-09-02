#ifndef MOTORS_CONST_HPP
#define MOTORS_CONST_HPP

#include <Arduino.h>
#include "Motors.hpp"

// それぞれのチャンネルに接続したモータの角度を指定
void motors_init(int deg_1ch, int deg_2ch, int deg_3ch, int deg_4ch);

// 指定した方向に動かす（PD制御付き）
void motors_move(int deg, int abs_power);

/*ここのファイル内だけで使う関数*/

// degはロボット全体として進みたい方向
// powerはその方向へ進むときの合力の大きさ
void compute_motor_power(int deg, int power);

// index番目のモータの合力の大きさを返す
double get_motor_power(int index);

// index番目のモータの出力をx方向に分解した値を返す
double get_motor_x_power(int index);

// index番目のモータの出力をy方向に分解した値を返す
double get_motor_y_power(int index);

#endif