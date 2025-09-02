#include "compute.hpp"
// deg,distanceについて
int line_deg = -1;
double line_distance = -1;
// memory_degについて
int line_memory_deg = -1;
bool is_LINE_memory_exist[16] = {false}; // Memory版用記録配列

// ラインの角度取得1
int get_LINE_deg()
{
    return line_deg;
}

// ラインの距離取得
double get_LINE_distance()
{
    return line_distance;
}

// ラインの角度取得2
int get_LINE_memory_deg()
{
    return line_memory_deg;
}

// サイドライン右
bool get_LINE_side_right()
{
    return get_LINE_data(16);
}

// サイドライン左
bool get_LINE_side_left()
{
    return get_LINE_data(17);
}

// サイドライン後ろ
bool get_LINE_side_back()
{
    return get_LINE_data(18);
}

// ラインの角度計算
void LINE_compute_update()
{
    // distance,degについて
    if (!is_LINE_exist())
    {
        line_deg = -1;
        line_distance = -1;
    }
    else
    {
        double line_x = 0.0, line_y = 0.0;
        for (int i = 0; i < 16; i++)
        {
            if (get_LINE_data(i))
            {
                line_x += cos(radians(22.5 * i));
                line_y += sin(radians(22.5 * i));
            }
        }
        if (get_LINE_side_right())
        {
            line_x += cos(radians(270.0));
            line_y += sin(radians(270.0));
        }
        if (get_LINE_side_left())
        {
            line_x += cos(radians(90.0));
            line_y += sin(radians(90.0));
        }
        if (get_LINE_side_back())
        {
            line_x += cos(radians(180.0));
            line_y += sin(radians(180.0));
        }

        line_deg = (int)round(degrees(atan2(line_y, line_x)));
        if (line_deg < 0)
            line_deg += 360;
        line_distance = double(line_x * line_x + line_y * line_y);
    }

    // memory_degについて
    if (!is_LINE_exist())
    {
        for (int i = 0; i < 16; i++)
            is_LINE_memory_exist[i] = false;

        line_memory_deg = -1;
    }
    else
    {
        double line_x = 0.0, line_y = 0.0;
        for (int i = 0; i < 16; i++)
        {
            if (get_LINE_data(i))
                is_LINE_memory_exist[i] = true;

            line_x += cos(radians(22.5 * i)) * is_LINE_memory_exist[i];
            line_y += sin(radians(22.5 * i)) * is_LINE_memory_exist[i];
        }
        if (get_LINE_side_right())
        {
            line_x += cos(radians(270.0));
            line_y += sin(radians(270.0));
        }
        if (get_LINE_side_left())
        {
            line_x += cos(radians(90.0));
            line_y += sin(radians(90.0));
        }
        if (get_LINE_side_back())
        {
            line_x += cos(radians(180.0));
            line_y += sin(radians(180.0));
        }

        line_memory_deg = (int)degrees(atan2(line_y, line_x));
        if (line_memory_deg < 0)
            line_memory_deg += 360;
    }
}
