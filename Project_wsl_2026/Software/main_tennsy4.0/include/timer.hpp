#include <Arduino.h>

class Timer
{
private:
    int _reset_time = 0;         // リセット時の時間格納用
    int _stop_time = 0;          // 停止時の時間格納用
    int _now_time = 0;           //_reset_timeまたは_pause_timeからの時間格納用
    bool _is_timer_stop = false; // タイマーはストップされているかどうか

public:
    void reset() /// タイマーを初期化、0からカウントを始める
    {
        _reset_time = millis(); // リセット時の時間更新
    }

    int get_from_timer() // タイマーからの時間を取得
    {
        if (!_is_timer_stop) // タイマーが起動している場合
        {
            _now_time = millis() - _reset_time; // タイマーの時間更新
        }
        else // タイマーが停止している場合
        {
            _now_time = _stop_time - _reset_time; // タイマーを停止した時の_now_timeを格納
        }

        return _now_time;
    }

    void stop() // タイマーを一時停止させる
    {
        if (!_is_timer_stop) // タイマーが起動しているなら
        {
            _is_timer_stop = true; // タイマーをストップさせる
            _stop_time = millis(); // この関数が呼び出されたときの時間を格納
        }
    }

    void start() // タイマーを再開させる
    {
        if (_is_timer_stop) // タイマーが停止されているなら
        {
            _is_timer_stop = false;                            // ストップ解除
            _reset_time = millis() - _stop_time + _reset_time; // 再開時の時間-ストップ時の時間+リセット時の時間=どのくらい停止されていたか+リセット時の時間
        }
    }

    bool is_timer_stop() // タイマーは一時停止されているかどうか
    {
        return _is_timer_stop;
    }
};
