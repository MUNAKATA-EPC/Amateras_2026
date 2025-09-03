#include "Timer.hpp"

void Timer::reset()
{
    _resetTime = millis();
    _isStopped = false;
}

void Timer::stop()
{
    if (!_isStopped)
    {
        _isStopped = true;
        _stopTime = millis();
    }
}

void Timer::start()
{
    if (_isStopped)
    {
        _isStopped = false;
        _resetTime = millis() - _stopTime + _resetTime;
    }
}

unsigned long Timer::msTime()
{
    if (!_everCalled)
        _everCalled = true;

    if (!_isStopped)
    {
        _nowTime = millis() - _resetTime;
    }
    else
    {
        _nowTime = _stopTime - _resetTime;
    }

    return _nowTime;
}
