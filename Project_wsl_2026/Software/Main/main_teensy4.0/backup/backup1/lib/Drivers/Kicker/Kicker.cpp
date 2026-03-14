#include "Kicker.hpp"

Kicker::Kicker(uint8_t kickpin, uint8_t chargepin)
{
    _kickpin = kickpin;
    _chargepin = chargepin;
}

bool Kicker::begin(unsigned long cooldownTime)
{
    _cooldownTime = cooldownTime;
    pinMode(_kickpin, OUTPUT);
    pinMode(_chargepin, OUTPUT);
    digitalWrite(_kickpin, LOW);
    digitalWrite(_chargepin, HIGH);

    return true;
}

void Kicker::kick(bool signal)
{
    if (signal && (_kickTimer.msTime() > _cooldownTime || !_kickTimer.everCalled()))
    {
        digitalWrite(_kickpin, HIGH);
        digitalWrite(_chargepin, LOW);
        _kickTimer.reset();
    }
    else if (_kickTimer.msTime() <= 200)
    {
        digitalWrite(_kickpin, HIGH);
        digitalWrite(_chargepin, LOW);
    }
    else
    {
        digitalWrite(_kickpin, LOW);
        digitalWrite(_chargepin, HIGH);
    }
}