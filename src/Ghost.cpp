#include "Ghost.h"
#include <GL/glut.h>
#include <random>

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

void Ghost::autoMove(float frameTimeMs) {
    bool _;
    //this->tryMoveToNextClosestTile();
    return;
}

MoveDir Ghost::randomDirection() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 3); // assuming 4 directions

    int random = dist(gen);
    switch (random) {
    case 0: return MoveDir::FWD;
    case 1: return MoveDir::BWD;
    case 2: return MoveDir::LEFT;
    case 3: return MoveDir::RIGHT;
    default: return MoveDir::NONE;
    }
}