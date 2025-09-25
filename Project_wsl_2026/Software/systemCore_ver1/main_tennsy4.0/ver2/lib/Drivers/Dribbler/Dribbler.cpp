#include "Dribbler.hpp"

Dribbler::Dribbler(uint8_t pin, int min, int max)
{
    _pin = pin;
    _min = min;
    _max = max;
}

void Dribbler::begin()
{
    _esc.attach(_pin);
}

void Dribbler::process()
{
    if (!_setupTimer.everCalled())
        _setupTimer.start();

    if (_setupTimer.msTime() < 4000)
    {
        _esc.writeMicroseconds(_min);

        _setup_flag = false;
    }
    else if (_setupTimer.msTime() < 8000)
    {
        _esc.writeMicroseconds(_max);

        _setup_flag = false;
    }
    else
    {
        _setup_flag = true;
    }
}

void Dribbler::move(int power)
{
    power = constrain(power, _min, _max);

    _esc.writeMicroseconds(power);
}