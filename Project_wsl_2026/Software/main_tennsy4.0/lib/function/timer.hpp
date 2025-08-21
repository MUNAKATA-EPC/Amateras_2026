#ifndef TIMER_HPP
#define TIMER_HPP

#include <Arduino.h>

class Timer
{
private:
    int32_t _reset_time = 0;         // リセット時の時間格納用
    int32_t _stop_time = -1;          // 停止時の時間格納用
    int32_t _now_time = 0;           //_reset_timeまたは_pause_timeからの時間格納用
    bool _is_timer_stop = false;      // タイマーはストップされているかどうか

public:
    // タイマーを初期化、0からカウントを始める
    void reset();

    // タイマーからの時間を取得
    int32_t get_time();

    // タイマーを一時停止させる
    void stop();

    // タイマーを再開させる
    void start();

    // タイマーは一時停止されているかどうか
    bool is_timer_stop();
};

#endif

