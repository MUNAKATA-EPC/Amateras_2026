#include "compute.hpp"

/*=== ノーマル版 ===*/
int16_t line_deg = -1;        // ラインの角度格納用（整数）
bool old_LINE_exist = false;   // 前回ラインがあったかどうか

/*=== Memory版 ===*/
int16_t line_memory_deg = -1; 
bool is_LINE_memory_exist[16] = {false}; // Memory版用記録配列

/*=== 共通 ===*/
int8_t first_triggered_pin = -1; // 最初に反応したピン
bool line_half_out = false;      // half_out判定用

/*=== 関数 ===*/
// ノーマル版の角度取得
int16_t get_LINE_deg()
{
    return line_deg;
}

// Memory版の角度取得
int16_t get_LINE_memory_deg()
{
    return line_memory_deg;
}

// 最初に反応したピン
int8_t get_first_triggered_pin()
{
    return first_triggered_pin;
}

// half_out判定
bool is_LINE_half_out()
{
    return line_half_out;
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

/*=== half_out判定関数 ===*/
bool check_half_out(uint8_t base_pin)
{
    uint8_t pinA = (base_pin + 4) % 16;

    for (uint8_t i = 1; i <= 7; i++)
    {
        uint8_t check_pin = (pinA + i) % 16;
        if (check_pin == base_pin) break;
        if (get_LINE_data(check_pin)) return true;
    }

    for (uint8_t i = 1; i <= 7; i++)
    {
        uint8_t check_pin = (pinA - i + 16) % 16;
        if (check_pin == base_pin) break;
        if (get_LINE_data(check_pin)) return true;
    }

    return false;
}

/*=== update関数（ノーマル＆Memory 同時計算） ===*/
void LINE_compute_update()
{
    /*--- ノーマル版計算 ---*/
    if (!is_LINE_exist())
    {
        line_deg = -1;
    }
    else
    {
        float line_x = 0.0f, line_y = 0.0f;
        for (uint8_t i = 0; i < 16; i++)
        {
            if (get_LINE_data(i))
            {
                line_x += cosf(radians(22.5f * i));
                line_y += sinf(radians(22.5f * i));
            }
        }
        if (get_LINE_data(16)) { line_x += cosf(radians(270.0f)); line_y += sinf(radians(270.0f)); }
        if (get_LINE_data(17)) { line_x += cosf(radians(90.0f));  line_y += sinf(radians(90.0f));  }
        if (get_LINE_data(18)) { line_x += cosf(radians(180.0f)); line_y += sinf(radians(180.0f)); }

        // float 計算結果を整数に丸める
        line_deg = (int16_t)roundf(degrees(atan2f(line_y, line_x)));
        if (line_deg < 0) line_deg += 360;
    }

    /*--- Memory版計算 ---*/
    if (!is_LINE_exist())
    {
        for (uint8_t i = 0; i < 16; i++) is_LINE_memory_exist[i] = false;
        line_memory_deg = -1;
    }
    else
    {
        float line_x = 0.0f, line_y = 0.0f;
        for (uint8_t i = 0; i < 16; i++)
        {
            if (get_LINE_data(i)) is_LINE_memory_exist[i] = true;
        }
        for (uint8_t i = 0; i < 16; i++)
        {
            line_x += cosf(radians(22.5f * i)) * is_LINE_memory_exist[i];
            line_y += sinf(radians(22.5f * i)) * is_LINE_memory_exist[i];
        }
        if (get_LINE_data(16)) { line_x += cosf(radians(270.0f)); line_y += sinf(radians(270.0f)); }
        if (get_LINE_data(17)) { line_x += cosf(radians(90.0f));  line_y += sinf(radians(90.0f));  }
        if (get_LINE_data(18)) { line_x += cosf(radians(180.0f)); line_y += sinf(radians(180.0f)); }

        line_memory_deg = (int16_t)roundf(degrees(atan2f(line_y, line_x)));
        if (line_memory_deg < 0) line_memory_deg += 360;
    }

    /*--- 最初に反応したピンを記録 ---*/
    if (!old_LINE_exist && is_LINE_exist())
    {
        first_triggered_pin = -1;
        for (uint8_t i = 0; i < 16; i++)
        {
            if (get_LINE_data(i))
            {
                first_triggered_pin = i;
                break;
            }
        }
    }
    old_LINE_exist = is_LINE_exist();

    /*--- half_out判定 ---*/
    if (first_triggered_pin != -1)
        line_half_out = check_half_out((uint8_t)first_triggered_pin);
    else
        line_half_out = false;
}
