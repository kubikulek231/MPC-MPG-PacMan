#include "Player.h"
#include <GL/glut.h>

Player::Player(float initPosX, float initPosY) {
	posX = initPosX;
	posY = initPosY;
}

Player::Player() {
}

float Player::getPosX() const {
	return posX;
}
float Player::getPosY() const {
	return posY;
}

void Player::render() {
    glColor3f(1.0f, 1.0f, 0.0f);
    glPushMatrix();
    glTranslatef(posX, TILE_SIZE / 2.0f, posY);
    glutSolidSphere(TILE_SIZE / 2, 16, 16);
    glPopMatrix();
}

void Player::move(MoveDir moveDir, float frameTime, float speed) {
	internalMove(moveDir, speed * frameTime);
}

void Player::internalMove(MoveDir moveDir, float speed) {
	if (moveDir == MoveDir::FWD) {
		posX += speed;
		return;
	}
	if (moveDir == MoveDir::BWD) {
		posX -= speed;
		return;
	}
	if (moveDir == MoveDir::RIGHT) {
		posY += speed;
		return;
	}
	posY -= speed;
}
