#pragma once
#include <Arduino.h>

class Timer
{
private:
    unsigned long _reset_time = 0UL; // リセット時の時間
    unsigned long _stop_time = 0UL;  // 停止時の時間
    unsigned long _now_time = 0UL;   // 計測時間

    bool _is_stopped = false;  // タイマーは停止しているか
    bool _ever_called = false; // タイマーは今まで呼ばれたか

public:
    // 関数
    void reset();
    void stop();
    void start();

    // データの取得
    unsigned long msTime();

    bool isStopped() const { return _is_stopped; }
    bool everCalled() const { return _ever_called; }
};
