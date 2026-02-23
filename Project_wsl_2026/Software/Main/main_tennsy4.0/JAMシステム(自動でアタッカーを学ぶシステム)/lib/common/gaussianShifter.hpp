#pragma once

#include "Arduino.h"
#include <math.h>

// ガウス関数を用いてデータを平滑化する。radiusは平滑化の範囲、sigmaは平滑化の強さを表す。radiusが大きいほど広い範囲を平均化し、sigmaが小さいほど中心に近い値を重視する。
void gaussianShifter(float *data, int size, int radius, float sigma);