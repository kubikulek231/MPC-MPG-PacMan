#include "Player.h"
#include <GL/glut.h>

Player::Player() {
}

Player::Player(const Player& other) {
    this->boundingBox = other.boundingBox;
    this->origin = other.origin;
}

Player::Player(Point3D playerOrigin, BoundingBox3D playerBoundingBox) : Entity(playerOrigin, playerBoundingBox) {
}

void Player::render() {
    glColor3f(1.0f, 1.0f, 0.0f);
    glPushMatrix();

    Point3D centerPoint = getAbsoluteCenterPoint();
    // Sphere is defined by center point, so needs to be translated
    glTranslatef(centerPoint.x, centerPoint.y, centerPoint.z);
    glutSolidSphere(0.5f, 16, 16);

    glPopMatrix();

    renderBoundingBox();
    renderOrigin();
}