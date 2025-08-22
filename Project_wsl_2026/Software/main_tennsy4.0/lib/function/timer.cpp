#include "timer.hpp"

/*タイマーが使えるようになる関数*/

void Timer::reset() /// タイマーを初期化、0からカウントを始める
{
    _reset_time = millis(); // リセット時の時間更新
    _is_timer_stop = false; // リセットしたら必ず動く状態にする
}

int Timer::get_time() // タイマーからの時間を取得
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

void Timer::stop() // タイマーを一時停止させる
{
    if (!_is_timer_stop) // タイマーが起動しているなら
    {
        _is_timer_stop = true; // タイマーをストップさせる
        _stop_time = millis(); // この関数が呼び出されたときの時間を格納
    }
}

void Timer::start() // タイマーを再開させる
{
    if (_is_timer_stop) // タイマーが停止されているなら
    {
        _is_timer_stop = false;                            // ストップ解除
        _reset_time = millis() - _stop_time + _reset_time; // 再開時の時間-ストップ時の時間+リセット時の時間
    }
}

bool Timer::is_timer_stop() // タイマーは一時停止されているかどうか
{
    return _is_timer_stop;
}
