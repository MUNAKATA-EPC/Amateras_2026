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

double Vector::get_x() const
{
    return _vector_x;
}
double Vector::get_y() const
{
    return _vector_y;
}
int Vector::get_deg() const
{
    int deg = (int)round(degrees(atan2(_vector_y, _vector_x)));
    if (deg < 0)
        deg += 360;
    return deg;
}
double Vector::get_rad() const
{
    double rad = atan2(_vector_y, _vector_x);
    if (rad < 0)
        rad += 2 * PI;
    return rad;
}
double Vector::get_length() const
{
    return sqrt(_vector_x * _vector_x + _vector_y * _vector_y);
}

Vector Vector::operator+(const Vector &other) const
{
    return Vector(_vector_x + other.get_x(), _vector_y + other.get_y());
}
Vector Vector::operator-(const Vector &other) const
{
    return Vector(_vector_x - other.get_x(), _vector_y - other.get_y());
}
Vector Vector::operator*(const double &scale) const
{
    return Vector(_vector_x * scale, _vector_y * scale);
}
Vector Vector::operator/(const double &scale) const
{
    return Vector(_vector_x / scale, _vector_y / scale);
}