#pragma once

// 関数
int diffDeg(int deg1, int deg2); // 二つの角度の差を返す

int areaIndexFromDeg(int n, int deg);   // n分割したときのdegに最も近いエリアを返す
int degFromAreaIndex(int n, int index); // n分割したときのindex番目のエリアの角度を返す

enum class MapMode
{
    HIREI,
    NIJI
};
int mapDeg(int deg, int deg_in, int deg_out, MapMode mode); // 角度をin->outになるように変形させその角度を返す

// 4・8・16分割の時は特別定義
enum class Area4
{
    FRONT,
    RIGHT,
    BACK,
    LEFT
};
enum class Area8
{
    FRONT,
    FRONT_RIGHT,
    RIGHT,
    BACK_RIGHT,
    BACK,
    BACK_LEFT,
    LEFT,
    FRONT_LEFT
};
enum class Area16
{
    FRONT,
    FRONT_FRONT_RIGHT,
    FRONT_RIGHT,
    RIGHT_FRONT_RIGHT,
    RIGHT,
    RIGHT_BACK_RIGHT,
    BACK_RIGHT,
    BACK_BACK_RIGHT,
    BACK,
    BACK_BACK_LEFT,
    BACK_LEFT,
    LEFT_BACK_LEFT,
    LEFT,
    LEFT_FRONT_LEFT,
    FRONT_LEFT,
    FRONT_FRONT_LEFT
};
Area4 area4(int deg);
Area8 area8(int deg);
Area16 area16(int deg);
