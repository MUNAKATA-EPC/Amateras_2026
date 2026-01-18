#pragma once

#include "Arduino.h"

// n個の点から(n-1)次関数を生成する。nが偶数ならば奇関数的（原点対称）な挙動、nが奇数ならば偶関数的（y軸対称）な挙動をする。
float lagrangeShifter(int n, float pos[][2], float x);