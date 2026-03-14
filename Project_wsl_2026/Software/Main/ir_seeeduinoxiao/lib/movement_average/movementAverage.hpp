#pragma once

#include <Arduino.h>

#define AVERAGE_COUNT_LIMIT 20

class MovementAverage
{
private:
    int _average_count;
    int _add_index;
    double _data[AVERAGE_COUNT_LIMIT]; // float/doubleに変更
    bool _valid[AVERAGE_COUNT_LIMIT];  // 0xFFFFのようなマジックナンバーではなくフラグで管理
    bool _cant_compute;

public:
    MovementAverage();
    MovementAverage(int average_count);
    void reset();
    double add(double data);
    bool is_cant_compute();
};