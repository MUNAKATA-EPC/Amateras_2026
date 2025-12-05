#pragma once

#include "Arduino.h"

#define AVERAGE_COUNT_LIMIT 10

class Movement_average
{
private:
    int _average_count = AVERAGE_COUNT_LIMIT;
    float _data[10] = {0xFFFF};

    int _add_index = 0;

    bool _cant_compute = false;

public:
    void set(int average_count);

    void add(int data);
    void addNone();

    int output();

    bool cant_compute() const { return _cant_compute; }
};