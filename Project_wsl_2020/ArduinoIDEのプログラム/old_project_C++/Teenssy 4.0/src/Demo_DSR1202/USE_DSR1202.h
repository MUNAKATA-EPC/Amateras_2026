#ifndef USE_DSR1202_H

#define USE_DSR1202_H

#include "Arduino.h"

//DSRの初期設定
void DSR1202begin(int pin);

//DSRを動かすための関数
void DSR1202move(int a, int b, int c, int d);

#endif