#ifndef POINT3D_H
#define POINT3D_H

#include <cmath>

class Point3D {
public:
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    Point3D() {};
    Point3D(float x, float y, float z);
    Point3D(const Point3D& other);
    void move(float dx, float dy, float dz);
    void move(Point3D dPoint);
    float distanceTo(const Point3D& other) const;
    float distanceTo2D(const Point3D& other) const;
    float getAxisValue(char axis = 'x') const;
    void setAxisValue(char axis = 'x', float value = 0.0f);

    // Overload the + operator to add two Point3D objects
    Point3D operator+(const Point3D& other) const {
        return Point3D(x + other.x, y + other.y, z + other.z);
    }

    // Overload the - operator to subtract two Point3D objects
    Point3D operator-(const Point3D& other) const {
        return Point3D(x - other.x, y - other.y, z - other.z);
    }

    // Divide Point3D by a scalar
    Point3D operator/(float scalar) const {
        return Point3D(x / scalar, y / scalar, z / scalar);
    }

    // Optionally, you can also overload the += and -= operators
    Point3D& operator+=(const Point3D& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    Point3D& operator-=(const Point3D& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    bool operator==(const Point3D& other) const {
        constexpr float EPSILON = 1e-5f;
        return std::fabs(x - other.x) < EPSILON &&
            std::fabs(y - other.y) < EPSILON &&
            std::fabs(z - other.z) < EPSILON;
    }

    bool operator!=(const Point3D& other) const {
        return !(*this == other);
    }

    bool isZero() const {
        if (x != 0) return false;
        if (y != 0) return false;
        if (z != 0) return false;
        return true;
    }
};

#endif