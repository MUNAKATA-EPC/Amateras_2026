#include "movementAverage.hpp"

MovementAverage::MovementAverage()
{
    _average_count = AVERAGE_COUNT_LIMIT;
    reset();
}

MovementAverage::MovementAverage(int average_count)
{
    _average_count = constrain(average_count, 1, AVERAGE_COUNT_LIMIT);
    reset();
}

void MovementAverage::reset()
{
    for (int i = 0; i < AVERAGE_COUNT_LIMIT; i++)
    {
        _data[i] = 0.0;
        _valid[i] = false;
    }
    _add_index = 0;
    _cant_compute = true;
}

double MovementAverage::add(double data)
{
    _data[_add_index] = data;
    _valid[_add_index] = true;

    _add_index = (_add_index + 1) % _average_count; // 加算して次更新する

    double sum = 0.0; // 【重要】intからdoubleに変更
    int available_count = 0;

    for (int i = 0; i < _average_count; i++)
    {
        if (_valid[i])
        { // 有効なデータのみ加算
            sum += _data[i];
            available_count++;
        }
    }

    double ave = 0.0;
    if (available_count > 0)
    {
        ave = sum / available_count;
        _cant_compute = false;
    }
    else
    {
        _cant_compute = true;
    }

    return ave;
}

bool MovementAverage::is_cant_compute()
{
    return _cant_compute;
}