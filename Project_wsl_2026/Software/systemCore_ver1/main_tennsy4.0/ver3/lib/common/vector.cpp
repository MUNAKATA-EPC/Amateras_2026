#include "vector.hpp"

Vector::Vector()
{
    _vector_x = 0;
    _vector_y = 0;
}
Vector::Vector(double x0, double y0, double x1, double y1)
{
    _vector_x = x1 - x0;
    _vector_y = y1 - y0;
}
Vector::Vector(int deg, double length)
{
    double rad = radians(deg);

    _vector_x = length * cos(rad);
    _vector_y = length * sin(rad);
}

Vector Vector::operator+(const Vector &other) const
{
    return Vector(0.0, 0.0, _vector_x + other.x(), _vector_y + other.y());
}
Vector Vector::operator-(const Vector &other) const
{
    return Vector(0.0, 0.0, _vector_x - other.x(), _vector_y - other.y());
}
Vector Vector::operator*(const double &scale) const
{
    return Vector(0.0, 0.0, _vector_x * scale, _vector_y * scale);
}
Vector Vector::operator/(const double &scale) const
{
    return Vector(0.0, 0.0, _vector_x / scale, _vector_y / scale);
}

Vector getVec(int deg, double length)
{
    Vector vec(deg, length);
    return vec;
}