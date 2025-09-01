#pragma once
#include <Arduino.h>

class Timer
{
private:
    unsigned long _resetTime = 0; // リセット時の時間
    unsigned long _stopTime = 0;  // 停止時の時間
    unsigned long _nowTime = 0;   // 計測時間

public:
    // 関数
    void reset();
    void stop();
    void start();

    unsigned long getTime();

    // 変数
    bool isStopped = false;  // タイマーは停止しているか
    bool everCalled = false; // タイマーは今まで呼ばれたか
};
