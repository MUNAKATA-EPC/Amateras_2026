#include "compute.hpp"

/* 得たラインセンサーの情報をもとに計算 記憶する方式（Memory版）と瞬間計算方式（版）がある */

/*=== 通常版（瞬間計算）用変数 ===*/
double line_deg = -1;
bool old_LINE_exist = false;

/*=== Memory版（記録計算）用変数 ===*/
double line_memory_deg = -1;
bool is_LINE_memory_exist[16] = {false}; // Memory版用記録配列

/*=== 共通変数 ===*/
int first_triggered_pin = -1;
bool line_half_out = false;

/*=== 関数 ===*/
// ノーマル版
int get_LINE_deg()
{
    return (int)line_deg;
}

// メモリ版
int get_LINE_memory_deg()
{
    return (int)line_memory_deg;
}

// ハーフアウト判定について
int get_first_triggered_pin()
{
    return first_triggered_pin;
}
bool is_LINE_half_out()
{
    return line_half_out;
}

// サイドラインについて
bool get_LINE_side_right()
{
    return get_LINE_data(16);
}

bool get_LINE_side_left()
{
    return get_LINE_data(17);
}

bool get_LINE_side_back()
{
    return get_LINE_data(18);
}

/*=== Half Out 判定関数 ===*/
bool check_half_out(int base_pin)
{
    int pinA = (base_pin + 4) % 16;

    // pinAの次からbase_pinまで時計回りに7つのセンサーを調べる
    for (int i = 1; i <= 7; i++)
    {
        int check_pin = (pinA + i) % 16;
        if (check_pin == base_pin)
        {
            // base_pinに戻ったら終了（何も見つからなかった）
            break;
        }
        if (get_LINE_data(check_pin))
            return true;
    }

    // pinAの前からbase_pinまで反時計回りに7つのセンサーを調べる
    for (int i = 1; i <= 7; i++)
    {
        int check_pin = (pinA - i + 16) % 16;
        if (check_pin == base_pin)
        {
            // base_pinに戻ったら終了（何も見つからなかった）
            break;
        }
        if (get_LINE_data(check_pin))
            return true;
    }

    return false;
}


/*=== update（ノーマル＆メモリ 同時計算） ===*/
void LINE_compute_update()
{
    /*--- ノーマル版計算 ---*/
    if (!is_LINE_exist())
    {
        line_deg = -1;
    }
    else
    {
        double line_x = 0, line_y = 0;
        for (int i = 0; i < 16; i++)
        {
            if (get_LINE_data(i))
            {
                line_x += cos(radians(22.5 * i));
                line_y += sin(radians(22.5 * i));
            }
        }
        if (get_LINE_data(16))
        {
            line_x += cos(radians(270));
            line_y += sin(radians(270));
        }
        if (get_LINE_data(17))
        {
            line_x += cos(radians(90));
            line_y += sin(radians(90));
        }
        if (get_LINE_data(18))
        {
            line_x += cos(radians(180));
            line_y += sin(radians(180));
        }

        line_deg = degrees(atan2(line_y, line_x));
        if (line_deg < 0)
            line_deg += 360;
    }

    /*--- Memory版計算 ---*/
    if (!is_LINE_exist())
    {
        for (int i = 0; i < 16; i++)
            is_LINE_memory_exist[i] = false;
        line_memory_deg = -1;
    }
    else
    {
        double line_x = 0, line_y = 0;
        for (int i = 0; i < 16; i++)
        {
            if (get_LINE_data(i))
                is_LINE_memory_exist[i] = true;
        }
        for (int i = 0; i < 16; i++)
        {
            line_x += cos(radians(22.5 * i)) * is_LINE_memory_exist[i];
            line_y += sin(radians(22.5 * i)) * is_LINE_memory_exist[i];
        }
        if (get_LINE_data(16))
        {
            line_x += cos(radians(270));
            line_y += sin(radians(270));
        }
        if (get_LINE_data(17))
        {
            line_x += cos(radians(90));
            line_y += sin(radians(90));
        }
        if (get_LINE_data(18))
        {
            line_x += cos(radians(180));
            line_y += sin(radians(180));
        }

        line_memory_deg = degrees(atan2(line_y, line_x));
        if (line_memory_deg < 0)
            line_memory_deg += 360;
    }

    /*--- 最初のピン判定（共通） ---*/
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

    /*--- half_out 判定（共通） ---*/
    if (first_triggered_pin != -1)
        line_half_out = check_half_out(first_triggered_pin);
    else
        line_half_out = false;
}
