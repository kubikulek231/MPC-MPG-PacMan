#include "Player.h"
#include <GL/glut.h>
#include <chrono>
#include <iostream>

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
    glColor3f(playerBodyColorRed, playerBodyColorGreen, playerBodyColorBlue);

    glPushMatrix();

    Point3D centerPoint = getAbsoluteCenterPoint();
    // Sphere is defined by center point, so needs to be translated
    glTranslatef(centerPoint.x, centerPoint.y, centerPoint.z);
    glutSolidSphere(0.5f, 16, 16);

    glPopMatrix();

    renderBoundingBox();
    renderOrigin();
}

void Player::move(MoveDir requestedMoveDir, bool& isNewRequest, float frameTimeMs) {
    // Handle movement and teleportation
    this->MovableEntity::move(requestedMoveDir, isNewRequest, frameTimeMs);
    this->teleport(moveDir);

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

void Player::setIsInvincible() {
    uint64_t currentTime = getTimeMs();
    invincibleEndTimeMs = currentTime + invincibleEndTimeAfterMs;
    isInvincible = true;
}

uint64_t Player::getTimeMs() {
    return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}