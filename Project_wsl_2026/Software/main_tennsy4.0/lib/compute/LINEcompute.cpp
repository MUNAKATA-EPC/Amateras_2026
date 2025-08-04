#include "LINEcompute.hpp"

/*得たラインセンサーの状況から角度などを算出する*/

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
    double line_x = 0;
    double line_y = 0;
    // エンジェルラインについてx,yそれぞれの方向で加算
    for (int i = 0; i < 16; i++)
    {
        // 裏表逆なので位置がIRセンサーとは逆
        line_x += cos(radians(-22.5 * i)) * get_LINE_data(i);
        line_y += sin(radians(-22.5 * i)) * get_LINE_data(i);
    }
    // サイドラインについてx,yそれぞれの方向で加算
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
    // 角度算出
    double line_deg = degrees(atan2(line_y, line_x));

    return (int)line_deg;
}