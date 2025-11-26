#pragma once
#include <Arduino.h>

class Motor
{
private:
    uint8_t _inA_pin, _inB_pin, _pwm_pin;

public:
    void set_pin(uint8_t inA_pin, uint8_t inB_pin, uint8_t pwm_pin)
    {
        _inA_pin = inA_pin;
        _inB_pin = inB_pin;
        _pwm_pin = pwm_pin;
        pinMode(_inA_pin, OUTPUT);
        pinMode(_inB_pin, OUTPUT);
        pinMode(_pwm_pin, OUTPUT);
    }

    void move(int power)
    {
        power = constrain(power, -100, 100);
        int output = abs(power) * 1023.0 / 100.0;

        if (power > 0)
        {
            digitalWrite(_inA_pin, HIGH);
            digitalWrite(_inB_pin, LOW);
        }
        else
        {
            digitalWrite(_inA_pin, LOW);
            digitalWrite(_inB_pin, HIGH);
        }

        analogWrite(_pwm_pin, output);
    }

    void stop()
    {
        digitalWrite(_inA_pin, LOW);
        digitalWrite(_inB_pin, LOW);
        analogWrite(_pwm_pin, 0);
    }
};
