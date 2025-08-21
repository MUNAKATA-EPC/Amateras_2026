#ifndef MOTORS_BOOST_HPP
#define MOTORS_BOOST_HPP

#include <Arduino.h>
#include "Motors.hpp"

// それぞれのチャンネルに接続したモータの角度を指定
void motors_init(int16_t deg_1ch, int16_t deg_2ch, int16_t deg_3ch, int16_t deg_4ch);

// 指定した方向に動かす（PD制御付き）
void motors_move(int16_t deg, int16_t abs_power);

// PD制御のみをする
void motors_only_PD(int16_t max_pd_power);

// すべてのモータを停止させる
void motors_break();

/*ここのファイル内だけで使う関数*/

// degはロボット全体として進みたい方向
// powerはその方向へ進むときの合力の大きさ
void compute_motor_power(int16_t deg, int16_t power);

// index番目のモータの合力の大きさを返す
float get_motor_power(int8_t index);

#endif
