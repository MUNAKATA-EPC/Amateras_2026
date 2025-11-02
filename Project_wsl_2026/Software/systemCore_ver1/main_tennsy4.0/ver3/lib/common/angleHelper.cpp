#include "angleHelper.hpp"

// 角度を -180度から180度の範囲に正規化する関数
int normalizeDeg(int deg)
{
    int norm = (deg % 360 + 360) % 360;
    if (norm > 180)
        norm -= 360;
    return norm;
}

// 角度の差分を -180度から180度の範囲で計算する
int diffDeg(int deg1, int deg2)
{
    int diff = (deg1 - deg2); // 差分

    // 差分が360度を超えないように調整
    int mod = (diff % 360 + 360) % 360;
    if (mod > 180)
        mod -= 360;
    return mod;
}

// targetDegに対する接線方向のうち、enterDegに近い方を返す
int nearSeesenDeg(int targetDeg, int enterDeg)
{
    // targetDegに±90°を加算した角度を正規化する
    int sessen1Deg = normalizeDeg(targetDeg + 90);
    int sessen2Deg = normalizeDeg(targetDeg - 90); // targetDeg + 270 は targetDeg - 90 と同じ

    // 差分を -180〜180 の範囲で計算
    bool isSessen1DegNear = abs(diffDeg(sessen1Deg, enterDeg)) < abs(diffDeg(sessen2Deg, enterDeg));

    return isSessen1DegNear ? sessen1Deg : sessen2Deg;
}

// 角度からエリアインデックスを計算
int areaIndexFromDeg(int n, int deg)
{
    double area_size = 360.0 / n;

    // degを0〜359に変換（-180は180, 180は180として扱う）
    int positiveDeg = (deg + 360) % 360;

    // 元のロジック: (positiveDeg + area_size / 2.0) / area_size
    int index = int((positiveDeg + area_size / 2.0) / area_size) % n;
    return index;
}

// エリアインデックスからエリアの中心角度を計算
int degFromAreaIndex(int n, int index)
{
    double area_size = 360.0 / n;

    // 0〜359度の中心角を計算
    int positiveDeg = int(index * area_size + area_size / 2.0);

    // 0〜359 の角度を -180〜179 に正規化して返す
    return normalizeDeg(positiveDeg);
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

// マッピング関数
int mapDeg(int deg, int deg_in, int deg_out, MapMode mode)
{
    // deg_in は入力値の最大絶対値（例: 180）
    // deg_out は出力値の最大絶対値（例: 100）

    int output;
    double scale;
    switch (mode)
    {
    case MapMode::HIREI: // 比例 (y = a*x)
        scale = (double)deg_out / deg_in;
        output = (int)round(deg * scale);
        break;

    case MapMode::NIJI: // 二次関数 (y = a*x^2 * sign(x))
        scale = (double)deg_out / (deg_in * deg_in);
        output = (int)round(deg * abs(deg) * scale);
        break;

    default:
        output = deg;
        break;
    }

    return output;
}