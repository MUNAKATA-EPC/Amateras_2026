#include "angleHelper.hpp"

int diffDeg(int deg1, int deg2)
{
    deg1 = (deg1 + 360) % 360; // 0〜360度に変換
    deg2 = (deg2 + 360) % 360; // 0〜360度に変換

    int diff = (deg1 - deg2 + 360) % 360;
    if (diff > 180)
        diff -= 360;
    return diff;
}

int nearSeesenDeg(int targetDeg, int enterDeg)
{
    int sessen1Deg = (targetDeg + 90) % 360, sessen2Deg = (targetDeg + 270) % 360;

    bool isSessen1DegNear = abs(diffDeg(sessen1Deg, enterDeg)) < abs(diffDeg(sessen2Deg, enterDeg));

    return isSessen1DegNear ? sessen1Deg : sessen2Deg; // どっちの接線が入力された角度に近いか
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

int mapDeg(int deg, int deg_in, int deg_out, MapMode mode)
{
    // 0~360 -> -180~180にする
    if (deg > 180)
        deg -= 360;

    // 計算
    int output;
    double scale;
    switch (mode)
    {
    case MapMode::HIREI:
        scale = (double)deg_out / deg_in;
        output = deg * scale;
        break;

    case MapMode::NIJI:
        scale = (double)deg_out / (deg_in * deg_in);
        output = deg * deg * scale;
        break;

    default:
        output = deg;
        break;
    }

    //-180~180 -> 0~360にする
    if (output < 0)
        output += 360;

    return output;
}