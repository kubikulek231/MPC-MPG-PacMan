#ifndef GHOST_H
#define GHOST_H

#include "MovableEntity.h"

class Ghost : public MovableEntity {
private:
    static constexpr float DEFAULT_SPEED = 3.0f;
    static constexpr float DEFAULT_SNAP_DISTANCE = 0.05f;
    static const MoveDir DEFAULT_MOVE_DIR = MoveDir::NONE;
    static constexpr bool DEFAULT_DIR_CHANGE_REQUEST_EXPIRE = false;
    static constexpr float DEFAULT_DIR_CHANGE_REQUEST_EXPIRE_AFTER_MS = 1000;
    MoveDir randomDirection();
    MoveDir currentDirection = MoveDir::NONE;
public:
    Ghost();
    Ghost(const Ghost& other);
    Ghost(Map* map, Point3D ghostOrigin, BoundingBox3D ghostBoundingBox);
    void render();
    void autoMove(float frameTimeMs);
};

#endif // PLAYER_H
