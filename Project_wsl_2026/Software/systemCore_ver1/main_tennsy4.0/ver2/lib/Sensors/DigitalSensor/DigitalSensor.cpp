#include "DigitalSensor.hpp"

DigitalSensor::DigitalSensor(uint8_t pin)
{
    _pin = pin;
}

bool DigitalSensor::begin()
{
    pinMode(_pin, INPUT);

    return true;
}

bool DigitalSensor::read()
{
    return bool(digitalRead(_pin));
}
