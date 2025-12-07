#pragma once

#include <Arduino.h>

// 関数
int normalizeDeg(int deg); // 角度を -180度から180度の範囲に正規化する関数

int diffDeg(int deg1, int deg2); // 角度の差分を -180度から180度の範囲で計算する

int nearSeesenDeg(int target_deg, int enter_deg); // targetDegの接線の角度を出しその2つ角度の内enterDegに近いほうを返す

int areaIndexFromDeg(int n, int deg);   // 角度からエリアインデックスを計算
int degFromAreaIndex(int n, int index); // エリアインデックスからエリアの中心角度を計算

enum class MapMode
{
    HIREI,
    NIJI
};
int mapDeg(int deg, int deg_in, int deg_out, MapMode mode); // マッピング関数

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
