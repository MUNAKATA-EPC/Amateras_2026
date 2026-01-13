#include "digitalSensor.hpp"

bool DigitalSensor::init(uint8_t pin)
{
    _pin = pin;

    pinMode(_pin, INPUT);

    return true;
}

bool DigitalSensor::read()
{
    return bool(digitalRead(_pin));
}

DigitalSensor catchSensor; // 共通で使えるようにしておく
