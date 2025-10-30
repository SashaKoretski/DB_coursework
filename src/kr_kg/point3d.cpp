#include "point3d.h"
#include <cmath>

Point3D::Point3D(float _x, float _y, float _z, int _pointIndex)
    : x(_x), y(_y), z(_z), pointIndex(_pointIndex)
{
}

Point3D::Point3D(const Point3D& other)
    : x(other.x), y(other.y), z(other.z), pointIndex(other.pointIndex)
{
}

Point3D& Point3D::operator=(const Point3D& other)
{
    if (this != &other) {
        x = other.x;
        y = other.y;
        z = other.z;
        pointIndex = other.pointIndex;
    }
    return *this;
}

bool Point3D::operator==(const Point3D& other) const
{
    const float epsilon = 0.001f;
    return std::abs(x - other.x) < epsilon &&
           std::abs(y - other.y) < epsilon &&
           std::abs(z - other.z) < epsilon &&
           pointIndex == other.pointIndex;
}

bool Point3D::operator!=(const Point3D& other) const
{
    return !(*this == other);
}

Point3D Point3D::operator+(const Point3D& other) const
{
    return Point3D(x + other.x, y + other.y, z + other.z, 0);
}

Point3D Point3D::operator-(const Point3D& other) const
{
    return Point3D(x - other.x, y - other.y, z - other.z, 0);
}

Point3D Point3D::operator*(float scalar) const
{
     
    return Point3D(x * scalar, y * scalar, z * scalar, pointIndex);
}

Point3D Point3D::operator/(float scalar) const
{
    if (std::abs(scalar) < 0.001f) {
        return Point3D(0.0f, 0.0f, 0.0f, pointIndex);
    }
     
    return Point3D(x / scalar, y / scalar, z / scalar, pointIndex);
}

Point3D& Point3D::operator+=(const Point3D& other)
{
    x += other.x;
    y += other.y;
    z += other.z;

    return *this;
}

Point3D& Point3D::operator-=(const Point3D& other)
{
    x -= other.x;
    y -= other.y;
    z -= other.z;

    return *this;
}

Point3D& Point3D::operator*=(float scalar)
{
    x *= scalar;
    y *= scalar;
    z *= scalar;

    return *this;
}

Point3D& Point3D::operator/=(float scalar)
{
    if (std::abs(scalar) >= 0.001f) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
    } else {
        x = y = z = 0.0f;
    }
    return *this;
}


int Point3D::getIndex() const
{
    return pointIndex;
}

void Point3D::setIndex(int index)
{
    if (index >= 1) {
        pointIndex = index;
    } else {
        pointIndex = 0;
    }
}

float Point3D::length() const
{
    return std::sqrt(x * x + y * y + z * z);
}

float Point3D::lengthSquared() const
{
    return x * x + y * y + z * z;
}

Point3D Point3D::normalized() const
{
    float len = length();
    if (len < 0.001f) {
        return Point3D(0.0f, 0.0f, 0.0f, pointIndex);
    }
    return Point3D(x / len, y / len, z / len, pointIndex);
}

void Point3D::normalize()
{
    float len = length();
    if (len >= 0.001f) {
        x /= len;
        y /= len;
        z /= len;
    } else {
        x = y = z = 0.0f;
    }
     
}

float Point3D::distance(const Point3D& p1, const Point3D& p2)
{
    return (p2 - p1).length();
}

float Point3D::distanceSquared(const Point3D& p1, const Point3D& p2)
{
    return (p2 - p1).lengthSquared();
}

float Point3D::dotProduct(const Point3D& p1, const Point3D& p2)
{
    return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}

Point3D Point3D::crossProduct(const Point3D& p1, const Point3D& p2)
{
     
    return Point3D(
        p1.y * p2.z - p1.z * p2.y,
        p1.z * p2.x - p1.x * p2.z,
        p1.x * p2.y - p1.y * p2.x,
        0
        );
}

Point3D Point3D::toXY() const
{
     
    return Point3D(x, y, 0.0f, pointIndex);
}

Point3D Point3D::toXZ() const
{
    return Point3D(x, 0.0f, z, pointIndex);
}

Point3D Point3D::toYZ() const
{
    return Point3D(0.0f, y, z, pointIndex);
}

void Point3D::setCoordinates(float _x, float _y, float _z)
{
    x = _x;
    y = _y;
    z = _z;
}

bool Point3D::isZero(float epsilon) const
{
    return std::abs(x) < epsilon && std::abs(y) < epsilon && std::abs(z) < epsilon;
}

bool Point3D::operator<(const Point3D& other) const
{
    if (x != other.x) return x < other.x;
    if (y != other.y) return y < other.y;
    if (z != other.z) return z < other.z;
    return pointIndex < other.pointIndex;
}
