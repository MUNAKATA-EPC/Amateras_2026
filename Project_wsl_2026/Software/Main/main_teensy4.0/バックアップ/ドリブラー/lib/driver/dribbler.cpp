#include "dribbler.hpp"

bool Dribbler::init(uint8_t pin, int min, int max)
{
    _pin = pin;
    _min = min;
    _max = max;
    if (_esc.attach(_pin, _min, _max))
    {
        _esc.writeMicroseconds(_min);
        return true;
    }

    return false;
}

void Dribbler::initMoving()
{
    if (!_flag)
    {
        _flag = true;
        _start_time = millis();
    }

    if (millis() - _start_time < 2000)
    {
        _esc.writeMicroseconds(_max);
    }
    else if (millis() - _start_time < 4000)
    {
        _esc.writeMicroseconds(_min);
    }
}

bool Dribbler::isInitMoving()
{
    return !_flag || (millis() - _start_time < 4000);
}

void Dribbler::move(int power)
{
    int safe_power = constrain(power, _min, _max);
    _esc.writeMicroseconds(safe_power);
}

Dribbler dribbler1;