#include "kicker.hpp"

bool Kicker::init(uint8_t kickpin, uint8_t chargepin, unsigned long cooldownTime)
{
    _kickpin = kickpin;
    _chargepin = chargepin;

    _cooldownTime = cooldownTime;
    pinMode(_kickpin, OUTPUT);
    pinMode(_chargepin, OUTPUT);
    digitalWrite(_kickpin, LOW);
    digitalWrite(_chargepin, HIGH);

    return true;
}

void Kicker::kick(bool signal)
{
    if (signal && (!_kickTimer.everReset() || _kickTimer.msTime() > _cooldownTime))
    {
        digitalWrite(_kickpin, HIGH);
        digitalWrite(_chargepin, LOW);
        _kickTimer.reset();
    }
    else if (_kickTimer.everReset() && _kickTimer.msTime() <= 200UL)
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

Kicker kicker1; // 共通で使えるようにしておく