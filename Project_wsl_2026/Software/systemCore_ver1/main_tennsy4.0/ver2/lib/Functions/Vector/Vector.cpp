#include "Vector.hpp"

Vector::Vector()
{
    _vector_x = 0;
    _vector_y = 0;
}
Vector::Vector(double x, double y)
{
    _vector_x = x;
    _vector_y = y;
}
Vector::Vector(int deg, double length)
{
    deg = (deg + 360) % 360; // 0~360にする
    double rad = radians(deg);

    _vector_x = length * cos(rad);
    _vector_y = length * sin(rad);
}

Vector Vector::operator+(const Vector &other) const
{
    return Vector(_vector_x + other.x(), _vector_y + other.y());
}
Vector Vector::operator-(const Vector &other) const
{
    return Vector(_vector_x - other.x(), _vector_y - other.y());
}
Vector Vector::operator*(const double &scale) const
{
    return Vector(_vector_x * scale, _vector_y * scale);
}
Vector Vector::operator/(const double &scale) const
{
    return Vector(_vector_x / scale, _vector_y / scale);
}

Vector getVec(int deg, double length)
{
    Vector vec(deg, length);
    return vec;
}