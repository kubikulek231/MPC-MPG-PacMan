#include "Player.h"
#include <GL/glut.h>

Player::Player() {
}

Player::Player(float initPosX, float initPosY) : Entity(initPosX, initPosY) {
}

void Player::render() {
    glColor3f(1.0f, 1.0f, 0.0f);
    glPushMatrix();
    glTranslatef(pos.x, TILE_SIZE / 2.0f, pos.y);
    glutSolidSphere(TILE_SIZE / 2, 16, 16);
    glPopMatrix();
}