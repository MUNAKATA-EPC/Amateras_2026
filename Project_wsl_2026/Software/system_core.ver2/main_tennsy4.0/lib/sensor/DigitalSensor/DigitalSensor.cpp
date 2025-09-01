#include "DigitalSensor.hpp"

DigitalSensor::DigitalSensor() {}

void DigitalSensor::init(uint8_t pin)
{
    _pin = pin;

    pinMode(_pin, INPUT);
}

bool DigitalSensor::get(){
    return digitalRead(_pin);
}