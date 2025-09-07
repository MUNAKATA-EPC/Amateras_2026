#include "Deg.hpp"

int diffDeg(int deg1, int deg2)
{
    int diff = (deg1 - deg2 + 360) % 360;
    if (diff > 180)
        diff -= 360;
    return diff;
}

int areaIndexFromDeg(int n, int deg)
{
    deg = (deg + 360) % 360;
    double area_size = 360.0 / n;
    int index = int((deg + area_size / 2.0) / area_size) % n;
    return index;
}

int degFromAreaIndex(int n, int index)
{
    double area_size = 360.0 / n;
    int deg = int((index * area_size + area_size / 2.0)) % 360;
    return deg;
}

Area4 area4(int deg)
{
    return Area4(areaIndexFromDeg(4, deg));
}

Area8 area8(int deg)
{
    return Area8(areaIndexFromDeg(8, deg));
}

Area16 area16(int deg)
{
    return Area16(areaIndexFromDeg(16, deg));
}
