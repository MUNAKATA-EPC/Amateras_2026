#include "Deg.hpp"

int diffDeg(int deg1, int deg2)
{
    int diff = (deg1 - deg2 + 360) % 360;
    if (diff > 180)
        diff -= 360;
    return diff;
}

int areaIndexFromDeg(int n, int deg){
    deg = (deg + 360) % 360;                                    // 0〜359に正規化
    double area_size = 360.0 / n;                               // 1エリアの幅
    int index = (int)((deg + area_size / 2.0) / area_size) % n; // 最近接のエリア番号
    return index;
}

int degFromAreaIndex(int n, int index)
{
    double area_size = 360.0 / n;                                 // 1エリアの幅
    int deg = (int)((index * area_size + area_size / 2.0)) % 360; // 中心角
    return deg;
}

int degSection4(int deg)
{
    deg = (deg + 360) % 360;
    if (deg >= 315 || deg < 45)
        return FRONT;
    else if (deg >= 45 && deg < 135)
        return RIGHT;
    else if (deg >= 135 && deg < 225)
        return BACK;
    else // if(deg >= 225 && deg < 315)
        return LEFT;
}

int degSection8(int deg)
{
    deg = (deg + 360) % 360;
    if (deg >= 337.5 || deg < 22.5)
        return FRONT;
    else if (deg >= 22.5 && deg < 67.5)
        return FRONT_RIGHT;
    else if (deg >= 67.5 && deg < 112.5)
        return RIGHT;
    else if (deg >= 112.5 && deg < 157.5)
        return BACK_RIGHT;
    else if (deg >= 157.5 && deg < 202.5)
        return BACK;
    else if (deg >= 202.5 && deg < 247.5)
        return BACK_LEFT;
    else if (deg >= 247.5 && deg < 292.5)
        return LEFT;
    else // if(deg >= 292.5 && deg < 337.5)
        return FRONT_LEFT;
}

int degSection16(int deg)
{
    deg = (deg + 360) % 360;
    if (deg >= 348.75 || deg < 11.25)
        return FRONT;
    else if (deg >= 11.25 && deg < 33.75)
        return FRONT_FRONT_RIGHT;
    else if (deg >= 33.75 && deg < 56.25)
        return FRONT_RIGHT;
    else if (deg >= 56.25 && deg < 78.75)
        return RIGHT_FRONT_RIGHT;
    else if (deg >= 78.75 && deg < 101.25)
        return RIGHT;
    else if (deg >= 101.25 && deg < 123.75)
        return RIGHT_BACK_RIGHT;
    else if (deg >= 123.75 && deg < 146.25)
        return BACK_RIGHT;
    else if (deg >= 146.25 && deg < 168.75)
        return BACK_BACK_RIGHT;
    else if (deg >= 168.75 && deg < 191.25)
        return BACK;
    else if (deg >= 191.25 && deg < 213.75)
        return BACK_BACK_LEFT;
    else if (deg >= 213.75 && deg < 236.25)
        return BACK_LEFT;
    else if (deg >= 236.25 && deg < 258.75)
        return LEFT_BACK_LEFT;
    else if (deg >= 258.75 && deg < 281.25)
        return LEFT;
    else if (deg >= 281.25 && deg < 303.75)
        return LEFT_FRONT_LEFT;
    else if (deg >= 303.75 && deg < 326.25)
        return FRONT_LEFT;
    else // if(deg >= 326.25 && deg < 348.75)
        return FRONT_FRONT_LEFT;
}