#include "timer.hpp"

void Timer::reset()
{
    _reset_time = millis();
    _is_stopped = false;
}

void Timer::stop()
{
    if (!_is_stopped)
    {
        _is_stopped = true;
        _stop_time = millis();
    }
}

void Timer::start()
{
    if (_is_stopped)
    {
        _is_stopped = false;
        _reset_time = millis() - _stop_time + _reset_time;
    }
}

unsigned long Timer::msTime()
{
    if (!_ever_called)
        _ever_called = true;

    if (!_is_stopped)
    {
        _now_time = millis() - _reset_time;
    }
    else
    {
        _now_time = _stop_time - _reset_time;
    }

    return _now_time;
}
