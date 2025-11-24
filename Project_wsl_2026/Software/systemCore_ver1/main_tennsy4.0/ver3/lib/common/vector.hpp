#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <Arduino.h>

class Vector
{
private:
    float _vector_x; // ベクトルのx方向
    float _vector_y; // ベクトルのy方向

public:
    // コンストラクタ
    Vector();
    Vector(float x0, float y0, float x1, float y1); // ベクトル生成 方法1
    Vector(int deg, float length);                     // ベクトル生成 方法2

    // 演算
    Vector operator+(const Vector &other) const; // ベクトル同士の足し算
    Vector operator-(const Vector &other) const; // ベクトル同士の引き算
    Vector operator*(const float &scale) const; // ベクトル同士の掛け算
    Vector operator/(const float &scale) const; // ベクトル同士の割り算

    // データの取得
    float x() const { return _vector_x; }                                                // x
    float y() const { return _vector_y; }                                                // y
    float length() const { return sqrtf(_vector_x * _vector_x + _vector_y * _vector_y); } // 大きさ

    int deg() const // 度数法[0~180]
    {
        int deg = (int)round(degrees(atan2f(_vector_y, _vector_x)));
        return deg;
    }

    float rad() const // ラジアン[0~2π]
    {
        float rad = atan2f(_vector_y, _vector_x);
        if (rad < 0.0f)
            rad += 2.0f * PI;
        return rad;
    }
};

// ベクター自動生成用
Vector getVec(int deg, float length);

#endif