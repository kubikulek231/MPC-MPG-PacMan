#ifndef ENTITY_H
#define ENTITY_H

#include "BoundingBox3D.h"
#include <GL/glut.h>  // For OpenGL rendering
#include <string>

// Each entity is defined by it's origin and bounding box
// Origin is always bottom left corner.
// Bounding box is defined relative to the origin.
// To get absolute Bounding box, call getAbsoluteBounding box method.
// To check if 2 entities intersect, call intersects.
class Entity {
protected:
    Point3D origin = Point3D();
    BoundingBox3D boundingBox = BoundingBox3D();
    void setBoundingBox(Point3D newMin, Point3D newMax);
    void renderText(const std::string& text) const;

public:
    Entity() {};
    Entity(Point3D origin, BoundingBox3D boundingBox);
    Entity(const Entity& other);

    Point3D getOrigin() const;
    BoundingBox3D getBoundingBox() const;
    BoundingBox3D getAbsoluteBoundingBox() const;

    Point3D getMovedOrigin(Point3D dPoint) const;
    BoundingBox3D getMovedBoundingBox(Point3D dPoint) const;
    BoundingBox3D getAbsoluteMovedBoundingBox(Point3D dpoint) const;
    
    Point3D getAbsoluteCenterPoint() const;

    void setOrigin(Point3D newOrigin);

    bool intersects(const Entity& otherEntity) const;

    // Debugging functions to render bounding box and origin
    void renderBoundingBox() const;
    void renderOrigin(bool renderCoordinates = false) const;
};

#endif