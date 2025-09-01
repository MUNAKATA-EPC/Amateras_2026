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

unsigned long Timer::millis()
{
    if (!_everCalled)
        _everCalled = true;

    if (!isStopped)
    {
        _nowTime = millis() - _resetTime;
    }
    else
    {
        _nowTime = _stopTime - _resetTime;
    }

    return _nowTime;
}
