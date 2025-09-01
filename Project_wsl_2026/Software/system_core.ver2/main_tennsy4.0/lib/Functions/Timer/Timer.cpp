#include "Timer.hpp"

void Timer::reset()
{
    _resetTime = millis();
    isStopped = false;
}

void Timer::stop()
{
    if (!isStopped)
    {
        isStopped = true;
        _stopTime = millis();
    }
}

void Timer::start()
{
    if (isStopped)
    {
        isStopped = false;
        _resetTime = millis() - _stopTime + _resetTime;
    }
}

unsigned long Timer::getTime()
{
    if (!everCalled)
        everCalled = true;

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
