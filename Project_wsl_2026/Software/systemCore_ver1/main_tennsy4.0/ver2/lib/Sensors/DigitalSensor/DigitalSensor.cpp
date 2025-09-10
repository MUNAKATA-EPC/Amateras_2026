#include "DigitalSensor.hpp"

DigitalSensor::DigitalSensor(uint8_t pin)
{
    _pin = pin;
}

void DigitalSensor::begin()
{
    pinMode(_pin, INPUT);
}

bool DigitalSensor::read()
{
    return bool(digitalRead(_pin));
}
