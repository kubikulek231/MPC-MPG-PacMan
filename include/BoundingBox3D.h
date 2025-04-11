#ifndef BOUNDINGBOX3D_H
#define BOUNDINGBOX3D_H

#include "Point3D.h"

class BoundingBox3D {
public:
    Point3D min = Point3D(0.f, 0.f, 0.f);
    Point3D max = Point3D(0.f, 0.f, 0.f);

    BoundingBox3D() {};
    BoundingBox3D(Point3D min, Point3D max);
    BoundingBox3D(const BoundingBox3D& other);
    void move(float dx, float dy, float dz);
    void move(Point3D dPoint);
    // Check if two BoundingBox3D objects intersect
    bool intersects(const BoundingBox3D& other) const;
    float distanceTo(const BoundingBox3D& other) const;
    BoundingBox3D getAbsoluteBoundingBox(Point3D bottomLeftOrigin) const;

    Point3D getFrontBottomLeft(const Point3D& origin = Point3D()) const;
    Point3D getFrontBottomRight(const Point3D& origin = Point3D()) const;
    Point3D getFrontTopLeft(const Point3D& origin = Point3D()) const;
    Point3D getFrontTopRight(const Point3D& origin = Point3D()) const;
    Point3D getBackBottomLeft(const Point3D& origin = Point3D()) const;
    Point3D getBackBottomRight(const Point3D& origin = Point3D()) const;
    Point3D getBackTopLeft(const Point3D& origin = Point3D()) const;
    Point3D getBackTopRight(const Point3D& origin = Point3D()) const;
    float getSizeX() const;
    float getSizeY() const;
    float getSizeZ() const;
};

#endif