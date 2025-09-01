#pragma once
#include <Arduino.h>

class Timer
{
private:
    unsigned long _resetTime = 0; // リセット時の時間
    unsigned long _stopTime = 0;  // 停止時の時間
    unsigned long _nowTime = 0;   // 計測時間

    bool _isStopped = false;  // タイマーは停止しているか
    bool _everCalled = false; // タイマーは今まで呼ばれたか

public:
    // 関数
    void reset();
    void stop();
    void start();

    // データの取得
    unsigned long msTime();

    bool isStopped() const { return _isStopped; }
    bool everCalled() const { return everCalled; }
};
