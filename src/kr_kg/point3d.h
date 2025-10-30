#ifndef POINT3D_H
#define POINT3D_H

class Point3D
{
public:
    float x, y, z;
    int pointIndex;

    Point3D(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f, int _pointIndex = 0);
    Point3D(const Point3D& other);

    Point3D& operator=(const Point3D& other);

    bool operator==(const Point3D& other) const;
    bool operator!=(const Point3D& other) const;

    Point3D operator+(const Point3D& other) const;
    Point3D operator-(const Point3D& other) const;
    Point3D operator*(float scalar) const;
    Point3D operator/(float scalar) const;

    Point3D& operator+=(const Point3D& other);
    Point3D& operator-=(const Point3D& other);
    Point3D& operator*=(float scalar);
    Point3D& operator/=(float scalar);

    bool operator<(const Point3D& other) const;

    int getIndex() const;
    void setIndex(int index);

    float length() const;
    float lengthSquared() const;
    Point3D normalized() const;
    void normalize();

    static float distance(const Point3D& p1, const Point3D& p2);
    static float distanceSquared(const Point3D& p1, const Point3D& p2);
    static float dotProduct(const Point3D& p1, const Point3D& p2);
    static Point3D crossProduct(const Point3D& p1, const Point3D& p2);

    Point3D toXY() const;    
    Point3D toXZ() const;    
    Point3D toYZ() const;    

    void setCoordinates(float _x, float _y, float _z);
    bool isZero(float epsilon = 0.001f) const;

    ~Point3D() = default;
};

#endif  
