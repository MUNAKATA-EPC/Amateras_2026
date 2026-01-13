#pragma once

#include "Arduino.h"

#define AVERAGE_COUNT_LIMIT 10

class MovementAverage
{
private:
    int _average_count = AVERAGE_COUNT_LIMIT;
    float _data[10] = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF};

    int _add_index = 0;

    bool _cant_compute = false;

public:
    MovementAverage(int average_count);

    void reset();

    float add(float data);
};