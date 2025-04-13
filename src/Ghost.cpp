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
    glColor3f(colorR, colorG, colorB);
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
    bool moved = false;
    bool turning = false;
    bool canTurn = false;
    std::vector<Tile*> tiles = intersectingTiles(this);
    while (true) {
        if (moveDir == MoveDir::NONE) {
            moveDir = randomDirection();
            continue;
        }
        if (randomBoolWithChance(0.1)) {
            turning = true;
        }

        if (turning) {
            this->preciseMoveUntilCanTurn(moveDir, frameTimeMs, canTurn, moved, tiles);
            if (canTurn) {
                // Choose a direction to turn
                moveDir = randomDirection();
                turning = false;
                return;
            }
            return;
        }

        this->preciseMove(moveDir, frameTimeMs, moved);
        if (moved) { break; }
        moveDir = randomDirection();
        if (randomBoolWithChance(0.05)) {
            turning = true;
        }
    }
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
    }
}

bool Ghost::randomBool() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    bool outBool;
    std::uniform_int_distribution<int> boolDist(0, 1);     // true or false
    outBool = boolDist(gen); // sets the reference param to true or false
    return outBool;
}

bool Ghost::randomBoolWithChance(float chance) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return dist(gen) < chance;
}