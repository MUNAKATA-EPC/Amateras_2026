#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <Arduino.h>

class Vector
{
private:
    double _vector_x; // ベクトルのx方向
    double _vector_y; // ベクトルのy方向

public:
    // コンストラクタ
    Vector();
    Vector(double x, double y, bool useXY);     // ベクトル生成 方法1
    Vector(int deg, double length); // ベクトル生成 方法2

    // 演算
    Vector operator+(const Vector &other) const; // ベクトル同士の足し算
    Vector operator-(const Vector &other) const; // ベクトル同士の引き算
    Vector operator*(const double &scale) const; // ベクトル同士の掛け算
    Vector operator/(const double &scale) const; // ベクトル同士の割り算

    // データの取得
    double x() const { return _vector_x; }                                                // x
    double y() const { return _vector_y; }                                                // y
    double length() const { return sqrt(_vector_x * _vector_x + _vector_y * _vector_y); } // 大きさ

    int deg() const // 度数法[0~180]
    {
        int deg = (int)round(degrees(atan2(_vector_y, _vector_x)));
        if (deg < 0)
            deg += 360;
        return deg;
    }

    double rad() const // ラジアン[0~2π]
    {
        double rad = atan2(_vector_y, _vector_x);
        if (rad < 0)
            rad += 2 * PI;
        return rad;
    }
};

// ベクター自動生成用
Vector getVec(int deg, double length);

#endif