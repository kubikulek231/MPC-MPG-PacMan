#include "Player.h"
#include <GL/glut.h>
#include <chrono>

using namespace std::chrono;

Player::Player() {
}

Player::Player(const Player& other) {
    this->boundingBox = other.boundingBox;
    this->origin = other.origin;
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
    if (isInvincible) {
        if (currentTimeMs > invincibleEndTimeMs) { isInvincible = false; }
        if (currentTimeMs > nextBlinkTimeMs) {
            invincibleBlink = !invincibleBlink;
            nextBlinkTimeMs = currentTimeMs + BLINK_DURATION_MS;
        }
        if (invincibleBlink) { playerBodyColorBlue = 1.0f; }
        else { playerBodyColorBlue = 0.0f; }
    }
}

void Player::setInvincible(uint64_t durationMs) {
    uint64_t currentTime = getTimeMs();
    invincibleEndTimeMs = currentTime + durationMs;
    isInvincible = true;
}

uint64_t Player::getTimeMs() {
    return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}