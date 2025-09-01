#include "bitSensor.hpp"

bitSensor::bitSensor() {}

void bitSensor::init(uint8_t pin)
{
    _pin = pin;

    pinMode(_pin, INPUT);
}

bool bitSensor::get(){
    return digitalRead(_pin);
}