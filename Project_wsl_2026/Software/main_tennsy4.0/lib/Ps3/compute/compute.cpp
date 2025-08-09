#include "compute.hpp"

/*ps3のデータを加工する*/

int ps3_stick_left_deg = -1;     // 左スティックの角度格納用
int ps3_stick_left_distance = 0; // 左スティックの距離格納用

int ps3_stick_right_deg = -1;     // 右スティックの角度格納用
int ps3_stick_right_distance = 0; // 右スティックの距離格納用

bool get_Ps3_button_up()
{
    return get_Ps3_button_data(0); // 0個目のデータを返す
}

bool get_Ps3_button_down()
{
    return get_Ps3_button_data(1); // 1個目のデータを返す
}

bool get_Ps3_button_left()
{
    return get_Ps3_button_data(2); // 2個目のデータを返す
}

bool get_Ps3_button_right()
{
    return get_Ps3_button_data(3); // 3個目のデータを返す
}

bool get_Ps3_button_triangle()
{
    return get_Ps3_button_data(4); // 4個目のデータを返す
}

bool get_Ps3_button_circle()
{
    return get_Ps3_button_data(5); // 5個目のデータを返す
}

bool get_Ps3_button_cross()
{
    return get_Ps3_button_data(6); // 6個目のデータを返す
}

bool get_Ps3_button_square()
{
    return get_Ps3_button_data(7); // 7個目のデータを返す
}

bool get_Ps3_button_l1()
{
    return get_Ps3_button_data(8); // 8個目のデータを返す
}

bool get_Ps3_button_l2()
{
    return get_Ps3_button_data(9); // 9個目のデータを返す
}

bool get_Ps3_button_l3()
{
    return get_Ps3_button_data(10); // 10個目のデータを返す
}

bool get_Ps3_button_r1()
{
    return get_Ps3_button_data(11); // 11個目のデータを返す
}

bool get_Ps3_button_r2()
{
    return get_Ps3_button_data(12); // 12個目のデータを返す
}

bool get_Ps3_button_r3()
{
    return get_Ps3_button_data(13); // 13個目のデータを返す
}

int get_Ps3_stick_left_deg()
{
    return ps3_stick_left_deg; // 左スティックの角度を返す
}

int get_Ps3_stick_left_distance()
{
    return ps3_stick_left_distance; // 左スティックの距離を返す
}

int get_Ps3_stick_right_deg()
{
    return ps3_stick_right_deg; // 右スティックの角度を返す
}

int get_Ps3_stick_right_distance()
{
    return ps3_stick_right_distance; // 右スティックの距離を返す
}

void Ps3_compute_update()
{
    if (is_Ps3_stick_left_move())
    {
        double ps3_stick_left_deg_sub = degrees(atan2(get_Ps3_stick_ly(), get_Ps3_stick_lx()));                        // 左スティックの角度を求める
        ps3_stick_left_deg_sub = (ps3_stick_left_deg_sub < 0) ? ps3_stick_left_deg_sub + 360 : ps3_stick_left_deg_sub; // 0~360に直す
        ps3_stick_left_deg = (int)ps3_stick_left_deg_sub;                                                              // 左スティックの角度を格納

        ps3_stick_left_distance = (int)sqrt(pow(get_Ps3_stick_lx(), 2) + pow(get_Ps3_stick_ly(), 2)); // 左スティックの距離を求める
        if (ps3_stick_left_distance > 128)                                                            // 128を超えたら128にする
            ps3_stick_left_distance = 128;
    }
    else
    {
        ps3_stick_left_deg = -1;
        ps3_stick_left_distance = 0;
    }

    if (is_Ps3_stick_right_move())
    {
        double ps3_stick_right_deg_sub = degrees(atan2(get_Ps3_stick_ry(), get_Ps3_stick_rx()));                           // 右スティックの角度を求める
        ps3_stick_right_deg_sub = (ps3_stick_right_deg_sub < 0) ? ps3_stick_right_deg_sub + 360 : ps3_stick_right_deg_sub; // 0~360に直す
        ps3_stick_right_deg = (int)ps3_stick_right_deg_sub;                                                                // 右スティックの角度を格納

        ps3_stick_right_distance = (int)sqrt(pow(get_Ps3_stick_rx(), 2) + pow(get_Ps3_stick_ry(), 2)); // 右スティックの距離を求める
        if (ps3_stick_right_distance > 128)                                                            // 128を超えたら128にする
            ps3_stick_right_distance = 128;
    }
    else
    {
        ps3_stick_right_deg = -1;
        ps3_stick_left_distance = 0;
    }
}