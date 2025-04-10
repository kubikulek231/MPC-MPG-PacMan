#ifndef MOVABLEENTITY_H
#define MOVABLEENTITY_H

#include <GL/glut.h>  // For OpenGL rendering
#include "Entity.h"

// Guess what this is.
class MovableEntity : public Entity {
public:
    MovableEntity() {};
    MovableEntity(Point3D origin, BoundingBox3D boundingBox);
    MovableEntity(const MovableEntity& other);

    void move(float dx, float dy, float dz);
    void move(Point3D dPoint);
    void moveX(float dx);
    void moveY(float dY);
    void moveZ(float dZ);
};

#endif