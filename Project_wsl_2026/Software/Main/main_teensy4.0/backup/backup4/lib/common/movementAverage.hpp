#pragma once

#include "Arduino.h"

#define AVERAGE_COUNT_LIMIT 10

// float型
class MovementAverage
{
private:
    int _average_count = AVERAGE_COUNT_LIMIT;
    float _data[10] = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF};

    int _add_index = 0;

    bool _is_able_to_compute = false;

public:
    MovementAverage(int average_count);

    void reset();

    float add(float data);
    float addEmpty();

    bool isAbleToCompute();
};

// double型
class MovementAverageDouble
{
private:
    int _average_count = AVERAGE_COUNT_LIMIT;
    double _data[10] = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF};

    int _add_index = 0;

    bool _is_able_to_compute = false;

public:
    MovementAverageDouble(int average_count);

    void reset();

    double add(double data);
    double addEmpty();

    bool isAbleToCompute();
};