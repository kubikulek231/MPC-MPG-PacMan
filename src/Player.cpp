#include "Player.h"
#include <GL/glut.h>
#include <chrono>
#include <iostream>
#include "Pi.h"

using namespace std::chrono;

Player::Player() {
}

Player::Player(const Player& other) {
    this->boundingBox = other.boundingBox;
    this->origin = other.origin;

    this->blinkDurationMs = DEFAULT_BLINK_DURATION_MS;
    this->invincibleEndTimeAfterMs = DEFAULT_INVINCIBLE_EXPIRE_AFTER_MS;
}

Player::Player(Map* map, Point3D playerOrigin, BoundingBox3D playerBoundingBox) 
: MovableEntity(map,
                playerOrigin,
                playerBoundingBox,
                Player::DEFAULT_MOVE_DIR,
                Player::DEFAULT_SPEED,
                Player::DEFAULT_SNAP_DISTANCE,
                Player::DEFAULT_DIR_CHANGE_REQUEST_EXPIRE,
                Player::DEFAULT_DIR_CHANGE_REQUEST_EXPIRE_AFTER_MS) {

    this->lastOrigin = this->origin;
    this->blinkDurationMs = DEFAULT_BLINK_DURATION_MS;
    this->invincibleEndTimeAfterMs = DEFAULT_INVINCIBLE_EXPIRE_AFTER_MS;
}

void Player::render() {
    glColor3f(playerBodyColorRed,
        playerBodyColorGreen,
        playerBodyColorBlue);

    glPushMatrix();
        Point3D c = getAbsoluteCenterPoint();
        glTranslatef(c.x, c.y + 0.25, c.z);

        // Rotate player to face movement direction
        float moveDirAngle = getMoveDirRotationAngle();
        glRotatef(moveDirAngle, 0.0f, 1.0f, 0.0f);

        // Align Pac-Man vertically (facing direction)
        glRotatef(90.0f, 0.0f, 0.0f, 1.0f);

        // Compute mouth opening angle
        float mouthDegrees = 30.0f * playerAnimationState;
        float inverseMouthDegrees = 180.0f - 60.0f - mouthDegrees;
        float half = inverseMouthDegrees * (PI / 180.0f);

        // Define clipping planes to carve the mouth
        GLdouble eq0[4] = { +sin(half), 0.0, -cos(half), 0.0 };
        GLdouble eq1[4] = { -sin(half), 0.0, -cos(half), 0.0 };

        // Render upper clipped sphere
        glClipPlane(GL_CLIP_PLANE0, eq0);
        glEnable(GL_CLIP_PLANE0);
        glDisable(GL_CLIP_PLANE1);
        glutSolidSphere(0.75f, 32, 32);
        glDisable(GL_CLIP_PLANE0);

        // Render lower clipped sphere
        glClipPlane(GL_CLIP_PLANE1, eq1);
        glEnable(GL_CLIP_PLANE1);
        glutSolidSphere(0.75f, 32, 32);
        glDisable(GL_CLIP_PLANE1);

        // Render inner mouth part (more orange)
        glColor3f(playerBodyColorRed,
            playerBodyColorGreen * 0.8f,
            playerBodyColorBlue);
        glPushMatrix();
        glClipPlane(GL_CLIP_PLANE1, eq1);
        glEnable(GL_CLIP_PLANE1);
        glutSolidSphere(0.75f, 32, 32);
        glDisable(GL_CLIP_PLANE1);
        glPopMatrix();

        // Fill the mouth gap with disks
        GLUquadric* disk = gluNewQuadric();
        glDisable(GL_CULL_FACE);
        glPushMatrix();
        glRotatef(inverseMouthDegrees, 0, 1, 0);
        gluDisk(disk, 0.0, 0.75, 32, 1);
        glPopMatrix();
        glPushMatrix();
        glRotatef(-inverseMouthDegrees, 0, 1, 0);
        gluDisk(disk, 0.0, 0.75, 32, 1);
        glPopMatrix();
        gluDeleteQuadric(disk);

        // --- LEFT EYE
        glColor3f(1.0f, 1.0f, 1.0f);
        glPushMatrix();
            glTranslatef(0.54f, 0.27f, -0.41f);

            glRotatef(35.0f, 0.0f, 0.0f, 1.0f);
            glRotatef(-55.0f, 0.0f, 1.0f, 0.0f);

            glScalef(1.0f, 1.0f, 0.3f);

            glutSolidSphere(0.20f, 12, 12);
        glPopMatrix();

        // LEFT PUPIL
        glColor3f(0.0f, 0.0f, 0.0f);
        glPushMatrix();
            glTranslatef(0.535f, 0.27f, -0.50f);

            glRotatef(40.0f, 0.0f, 0.0f, 1.0f);
            glRotatef(-44.5f, 0.0f, 1.0f, 0.0f);

            glScalef(1.0f, 1.0f, 0.3f);
            glutSolidSphere(0.11f, 12, 12);
        glPopMatrix();

        // --- RIGHT EYE
        glColor3f(1.0f, 1.0f, 1.0f);
        glPushMatrix();
            glTranslatef(0.54f, -0.27f, -0.41f);

            glRotatef(145.0f, 0.0f, 0.0f, 1.0f);
            glRotatef(-125.0f, 0.0f, 1.0f, 0.0f);

            glScalef(1.0f, 1.0f, 0.3f);

            glutSolidSphere(0.20f, 12, 12);
        glPopMatrix();

        // RIGHT PUPIL
        glColor3f(0.0f, 0.0f, 0.0f);
        glPushMatrix();
            glTranslatef(0.535f, -0.27f, -0.50f);

            glRotatef(140.0f, 0.0f, 0.0f, 1.0f);
            glRotatef(-135.5f, 0.0f, 1.0f, 0.0f);

            glScalef(1.0f, 1.0f, 0.3f);
            glutSolidSphere(0.11f, 12, 12);
        glPopMatrix();

    glPopMatrix();

    //renderBoundingBox();
    //renderOrigin();
}


void Player::move(MoveDir requestedMoveDir, bool& isNewRequest, float frameTimeMs) {
    // Handle movement and teleportation
    this->MovableEntity::move(requestedMoveDir, isNewRequest, frameTimeMs);
    this->teleport(moveDir);
    
    if (moveDir == MoveDir::UNDEFINED || moveDir == MoveDir::NONE) { return; }

    auto tiles = intersectingTiles(this);
    Tile* tileCurrent = !tiles.empty() ? currentTile(tiles) : nullptr;
    Tile* nextTileInDir = tileCurrent != nullptr ? nextTileInDirection(moveDir, tileCurrent) : nullptr;
    bool isPelletNext = nextTileInDir != nullptr && nextTileInDir->getTileType() == TileType::PELLET;

    animate(frameTimeMs, isPelletNext);

    //float newDistance = this->origin.distanceTo2D(this->lastOrigin);
    //if (speedoMeter.update(newDistance, 20)) { std::cout << "Move distance: " << speedoMeter.getAverage() << std::endl; }
    //this->lastOrigin = this->origin;
}

void Player::update(int& totalCollectedPellets) {
    uint64_t currentTimeMs = getTimeMs();

    // Collect pellets
    auto tiles = this->intersectingTiles(this);
    auto tile = currentTile(tiles);
    if (tile && map->collectPellet(tile)) {
        totalCollectedPellets++;
    }

    // Update invincibility and blinking
    if (getIsInvincible()) {
        if (currentTimeMs > invincibleEndTimeMs) { isInvincible = false; }
        if (currentTimeMs > nextBlinkTimeMs) {
            invincibleBlink = !invincibleBlink;
            nextBlinkTimeMs = currentTimeMs + blinkDurationMs;
        }
        if (invincibleBlink) { playerBodyColorBlue = 1.0f; }
        else { playerBodyColorBlue = 0.0f; }
    }
}

void Player::animate(float frameTimeMs, bool keepAnimating) {
    if (!playerMouthOpening && !playerMouthClosing)
        return;

    // dt in seconds
    float dt = frameTimeMs;

    float baseSpeed = speed * 2.0f;            // e.g. speed = 1.0f means 1.0 per second

    const float minFactor = 0.3f;       // never go slower than 30%
    float f = sinf(PI * playerAnimationState);
    float easeFactor = minFactor + (1.0f - minFactor) * f;

    // final delta
    float dstate = baseSpeed * dt * easeFactor;

    // apply to opening / closing
    if (playerMouthOpening) {
        playerAnimationState = std::min(playerAnimationState + dstate, 1.0f);
        if (playerAnimationState >= 1.0f) {
            playerMouthOpening = false;
            playerMouthClosing = true;
        }
        return;
    }

    // closing
    playerAnimationState = std::max(playerAnimationState - dstate, 0.0f);
    if (playerAnimationState <= 0.0f && keepAnimating) {
        playerMouthOpening = true;
        playerMouthClosing = false;
    }
}

void Player::setIsInvincible() {
    uint64_t currentTime = getTimeMs();
    invincibleEndTimeMs = currentTime + invincibleEndTimeAfterMs;
    isInvincible = true;
}

uint64_t Player::getTimeMs() {
    return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}