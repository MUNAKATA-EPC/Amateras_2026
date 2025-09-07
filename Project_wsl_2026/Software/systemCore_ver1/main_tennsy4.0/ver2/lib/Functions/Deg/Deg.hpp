/*角度は反時計周りに0->360と変化するものとしている*/
#pragma once

#include <Arduino.h>

// 角度と角度の差を計算する
int diffDeg(int deg1, int deg2);

// 反時計回りに n 分割したエリアの中心に最も近い角度のエリア番号を返す（Area0は4分割の場合0~45と315~360）
int areaIndexFromDeg(int n, int deg);

// 反時計回りに n 分割したエリアのindex番目のエリアの中心角を返す
int degFromAreaIndex(int n, int index);

// 角度を分割しどの方向にあるのか返す
int degToArea4(int deg); // 4分割
enum Area4
{
    FRONT = 0,
    RIGHT = 1,
    BACK = 2,
    LEFT = 3
};
int degToArea8(int deg); // 8分割
enum Area8
{
    FRONT = 0,
    FRONT_RIGHT = 1,
    RIGHT = 2,
    BACK_RIGHT = 3,
    BACK = 4,
    BACK_LEFT = 5,
    LEFT = 6,
    FRONT_LEFT = 7
};
int degToArea16(int deg); // 16分割
enum Area16
{
    FRONT = 0,
    FRONT_FRONT_RIGHT = 1,
    FRONT_RIGHT = 2,
    RIGHT_FRONT_RIGHT = 3,
    RIGHT = 4,
    RIGHT_BACK_RIGHT = 5,
    BACK_RIGHT = 6,
    BACK_BACK_RIGHT = 7,
    BACK = 8,
    BACK_BACK_LEFT = 9,
    BACK_LEFT = 10,
    LEFT_BACK_LEFT = 11,
    LEFT = 12,
    LEFT_FRONT_LEFT = 13,
    FRONT_LEFT = 14,
    FRONT_FRONT_LEFT = 15
};