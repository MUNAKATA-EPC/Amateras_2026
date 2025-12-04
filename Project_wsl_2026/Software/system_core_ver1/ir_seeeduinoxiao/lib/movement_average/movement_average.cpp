#include "movement_average.hpp"

void Movement_average::set(int average_count)
{
    _average_count = constrain(average_count, 1, AVERAGE_COUNT_LIMIT);
}

void Movement_average::add(float data)
{
    _data[_add_index] = data;

    _add_index = (_add_index + 1) % _average_count; // 加算して次更新する
}

void Movement_average::addNone()
{
    add(0xFFFF);
}

float Movement_average::output()
{
    float sum = 0.0f, available_count = 0;
    for (int i = 0; i < _average_count; i++)
    {
        if (_data[i] != 0xFFFF)
        {
            sum += _data[i];
            available_count++;
        }
    }

    float ave = 0.0f;
    if (available_count > 0)
    {
        ave = sum / available_count;

        _cant_compute = false;
    }
    else
    {
        _cant_compute = true;
    }

    return (int)round(ave);
}