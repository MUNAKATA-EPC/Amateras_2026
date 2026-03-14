#pragma once

#include <Arduino.h>

class Multiplexer
{
private:
    int _s0_pin, _s1_pin, _s2_pin, _s3_pin;
    int _read_pin, _enable_pin;
    int _multiplexer_settle_microseconds;

public:
    void set_pin(int s0_pin, int s1_pin, int s2_pin, int s3_pin, int read_pin, int enable_pin);
    void init(int multiplexer_settle_microseconds);
    int read(int index);
};