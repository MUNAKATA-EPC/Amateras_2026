#ifndef MOTORS_HPP
#define MOTORS_HPP

#include <Arduino.h>
#include "DSR1202.hpp"
#include "PDpower.hpp"

// それぞれのチャンネルに接続したモータの角度を指定
void motors_init(int deg_1ch, int deg_2ch, int deg_3ch, int deg_4ch);

// 指定した方向に動かす（PD制御付き）
void motors_move(int deg, int abs_power);

#endif