#include "DigitalSensor.hpp"

DigitalSensor::DigitalSensor(uint8_t pin)
{
    _pin = pin;
}

void DigitalSensor::begin()
{
    pinMode(_pin, INPUT);
}

DigitalSensor::ToggleSwitch DigitalSensor::readToggle()
{
    return ToggleSwitch(digitalRead(_pin));
}

DigitalSensor::CatchSensor DigitalSensor::readCatch()
{
    return CatchSensor(digitalRead(_pin));
}
