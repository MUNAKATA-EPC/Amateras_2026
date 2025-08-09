#include "compute.hpp"

/*得たラインセンサーの状況から角度などを算出する*/

bool old_LINE_exist = false;
bool is_LINE_exsit_memory[16] = {false}; // ラインセンサー記憶用配列（エンジェル）
double line_deg;                         // 角度格納用
int first_triggered_pin;                 // 最初のセンサーピン格納用
bool line_half_out;                      // 半分出たか検知用

bool get_LINE_side_right()
{
    return get_LINE_data(16); // 16個目のデータを返す
}

bool get_LINE_side_left()
{
    return get_LINE_data(17); // 17個目のデータを返す
}

bool get_LINE_side_back()
{
    return get_LINE_data(18); // 18個目のデータを返す
}

int get_LINE_deg()
{
    return (int)line_deg;
}

int get_first_triggered_pin()
{
    return first_triggered_pin; // 最初に反応したピンを返す
}

bool is_LINE_half_out()
{
    return line_half_out; // half_outしたかどうか返す
}

bool check_half_out(int base_pin)
{
    int pinA = (base_pin + 4) % 16;
    // int pinB = (base_pin - 4 + 16) % 16;

    // 弧A: base_pin を含まない方向
    bool passed_base = false;
    for (int i = 1; i <= 7; i++)
    {
        int check_pin = (pinA + i) % 16;
        if (check_pin == base_pin)
        {
            passed_base = true;
            break;
        }
        if (get_LINE_data(check_pin))
            return true;
    }

    if (passed_base)
    {
        // 弧B でも確認
        for (int i = 1; i <= 7; i++)
        {
            int check_pin = (pinA - i + 16) % 16;
            if (check_pin == base_pin)
                break;
            if (get_LINE_data(check_pin))
                return true;
        }
    }

    return false;
}

void LINE_compute_update()
{
    // === 角度計算 ===
    if (!is_LINE_exist())
    {
        for (int i = 0; i < 16; i++)
            is_LINE_exsit_memory[i] = false;
        line_deg = -1;
    }
    else
    {
        double line_x = 0, line_y = 0;
        for (int i = 0; i < 16; i++)
        {
            if (get_LINE_data(i))
                is_LINE_exsit_memory[i] = true;
        }
        for (int i = 0; i < 16; i++)
        {
            line_x += cos(radians(22.5 * i)) * is_LINE_exsit_memory[i];
            line_y += sin(radians(22.5 * i)) * is_LINE_exsit_memory[i];
        }
        if (get_LINE_side_right())
        {
            line_x += cos(radians(270));
            line_y += sin(radians(270));
        }
        if (get_LINE_side_left())
        {
            line_x += cos(radians(90));
            line_y += sin(radians(90));
        }
        if (get_LINE_side_back())
        {
            line_x += cos(radians(180));
            line_y += sin(radians(180));
        }

        line_deg = degrees(atan2(line_y, line_x));
        if (line_deg < 0)
            line_deg += 360;
    }

    // === 最初のピン判定 ===
    if (!old_LINE_exist && is_LINE_exist())
    {
        for (int i = 0; i < 16; i++)
        {
            if (get_LINE_data(i))
            {
                first_triggered_pin = i;
                break;
            }
        }
    }
    old_LINE_exist = is_LINE_exist();

    // === half_out 判定 ===
    if (first_triggered_pin != -1)
    {
        line_half_out = check_half_out(first_triggered_pin);
    }
    else
    {
        line_half_out = false;
    }
}