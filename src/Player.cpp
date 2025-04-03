#include "Player.h"
#include <GL/glut.h>
#include <cmath> // For any additional math functions


// Constructor to initialize player position
Player::Player(float initialPosX, float initialPosY) {
    posX = initialPosX;
    posY = initialPosY;
}

// Render the player at the current position
void Player::render() {
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow for Pacman
    glPushMatrix();
    glTranslatef(posX, TILE_SIZE / 2.0f, posY); // Translate to player's position
    glutSolidSphere(TILE_SIZE / 2, 8, 8); // Draw the player (Pacman)
    glPopMatrix();
}

// Move the player based on the direction and speed
void Player::move(MoveDir direction) {
    // Update the position based on direction
    if (direction == FWD) {
        posX += SPEED;
        return;
    }
    if (direction == BWD) {
        posX -= SPEED;
        return;
    }
    if (direction == RIGHT) {
        posY += SPEED;
        return;
    }
    posY -= SPEED;
}
