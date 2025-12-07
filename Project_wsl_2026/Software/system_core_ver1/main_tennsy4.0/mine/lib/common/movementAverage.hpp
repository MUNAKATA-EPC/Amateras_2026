#pragma once

#include "Arduino.h"

#define AVERAGE_COUNT_LIMIT 10

class MovementAverage
{
private:
    int _average_count = AVERAGE_COUNT_LIMIT;
    int _data[10] = {0xFFFF};

    int _add_index = 0;

    bool _cant_compute = false;

public:
    void set(int average_count);

    void reset();

    int add(int data);
};