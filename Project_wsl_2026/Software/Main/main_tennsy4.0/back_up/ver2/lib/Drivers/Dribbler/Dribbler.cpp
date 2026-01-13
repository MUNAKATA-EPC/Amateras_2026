#include "Dribbler.hpp"

Dribbler::Dribbler(uint8_t pin, int min, int max)
{
    _pin = pin;
    _min = min;
    _max = max;
}

bool Dribbler::begin()
{
    bool success = _esc.attach(_pin);

    return success;
}

void Dribbler::setup()
{
    if (!_setupTimer.everCalled())
        _setupTimer.start();

    if (_setupTimer.msTime() < 4000)
    {
        _esc.writeMicroseconds(_min);
        _setup_count = _max;
        _setup_flag = false;
    }
    else
    {
        if (!_setup_flag)
        {
            _esc.writeMicroseconds(_setup_count);
            _setup_count -= (_setupTimer.msTime() - 4000) / 80;
        }
        else
        {
            _esc.writeMicroseconds(_min);
        }

        if (_setup_count <= _min)
            _setup_flag = true;
    }
}

void Dribbler::move(int power)
{
    power = constrain(power, _min, _max);

    _esc.writeMicroseconds(power);
}