#include "Ghost.h"
#include <GL/glut.h>

Ghost::Ghost() {
}

Ghost::Ghost(const Ghost& other) {
    this->boundingBox = other.boundingBox;
    this->origin = other.origin;
}

Ghost::Ghost(Map* map, Point3D ghostOrigin, BoundingBox3D ghostBoundingBox) 
: MovableEntity(map,
                ghostOrigin,
                ghostBoundingBox,
                Ghost::DEFAULT_MOVE_DIR,
                Ghost::DEFAULT_SPEED,
                Ghost::DEFAULT_SNAP_DISTANCE,
                Ghost::DEFAULT_DIR_CHANGE_REQUEST_EXPIRE,
                Ghost::DEFAULT_DIR_CHANGE_REQUEST_EXPIRE_AFTER_MS) {
}

void Ghost::render() {
    glColor3f(0.0f, 1.0f, 0.0f);
    glPushMatrix();

    Point3D centerPoint = getAbsoluteCenterPoint();
    // Sphere is defined by center point, so needs to be translated
    glTranslatef(centerPoint.x, centerPoint.y, centerPoint.z);
    glutSolidSphere(0.5f, 16, 16);

    glPopMatrix();

    renderBoundingBox();
    renderOrigin();
}

void Ghost::move() {
    //this->move();
    return;
}