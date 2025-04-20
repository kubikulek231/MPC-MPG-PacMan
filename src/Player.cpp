#include "Player.h"
#include <GL/glut.h>

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

void Player::move(MoveDir requestedMoveDir, bool& isNewRequest, float frameTimeMs, int &totalCollectedPellets) {
    this->MovableEntity::move(requestedMoveDir, isNewRequest, frameTimeMs);
    this->teleport(moveDir);
    // Collect pellets
    auto tiles = this->intersectingTiles(this);
    auto tile = currentTile(tiles);
    if (tile && map->collectPellet(tile)) {
        totalCollectedPellets++;
    }
}