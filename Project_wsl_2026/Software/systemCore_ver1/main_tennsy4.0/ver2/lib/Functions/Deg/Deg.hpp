#pragma once

// 関数
int diffDeg(int deg1, int deg2); // 二つの角度の差を返す

int areaIndexFromDeg(int n, int deg);   // n分割したときのdegに最も近いエリアを返す
int degFromAreaIndex(int n, int index); // n分割したときのindex番目のエリアの角度を返す

// 4・8・16分割の時は特別定義
enum Area4
{
    FRONT,
    RIGHT,
    BACK,
    LEFT
};
enum Area8
{
    FRONT8,
    FRONT_RIGHT,
    RIGHT8,
    BACK_RIGHT,
    BACK8,
    BACK_LEFT,
    LEFT8,
    FRONT_LEFT8
};
enum Area16
{
    FRONT16,
    FRONT_FRONT_RIGHT,
    FRONT_RIGHT16,
    RIGHT_FRONT_RIGHT,
    RIGHT16,
    RIGHT_BACK_RIGHT,
    BACK_RIGHT16,
    BACK_BACK_RIGHT,
    BACK16,
    BACK_BACK_LEFT,
    BACK_LEFT16,
    LEFT_BACK_LEFT,
    LEFT16,
    LEFT_FRONT_LEFT,
    FRONT_LEFT16,
    FRONT_FRONT_LEFT16
};
Area4 area4(int deg);
Area8 area8(int deg);
Area16 area16(int deg);
