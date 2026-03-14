#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <Arduino.h>

class Vector
{
private:
    double _vector_x; // ベクトルのx方向
    double _vector_y; // ベクトルのy方向

public:
    Vector();                       // Vector Vなどとしても0が代入される
    Vector(double x, double y);     // ベクトル生成 方法1
    Vector(int deg, double length); // ベクトル生成 方法2

    double get_x() const;           // xを出力
    double get_y() const;           // yを出力
    int get_deg() const;            // ベクトルの方向を出力(0~360)
    double get_rad() const; // ラジアンで出力
    double get_length() const;      // ベクトルの大きさを出力

    Vector operator+(const Vector &other) const; // ベクトル同士の足し算
    Vector operator-(const Vector &other) const; // ベクトル同士の引き算
    Vector operator*(const double &scale) const;  // ベクトル同士の掛け算
    Vector operator/(const double &scale) const;  // ベクトル同士の割り算
};

#endif
