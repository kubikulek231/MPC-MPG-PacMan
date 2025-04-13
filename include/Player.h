#ifndef PLAYER_H
#define PLAYER_H

#include "MovableEntity.h"

class Player : public MovableEntity {
private:
    static constexpr float DEFAULT_SPEED = 1.2f;
    static constexpr float DEFAULT_SNAP_DISTANCE = 0.05f;
    static const MoveDir DEFAULT_MOVE_DIR = MoveDir::NONE;
    static constexpr bool DEFAULT_DIR_CHANGE_REQUEST_EXPIRE = false;
    static constexpr float DEFAULT_DIR_CHANGE_REQUEST_EXPIRE_AFTER_MS = 1000;
public:
    Player();
    Player(const Player& other);
    Player(Map* map, Point3D playerOrigin, BoundingBox3D playerBoundingBox);
    virtual void render();
};

#endif // PLAYER_H
