#ifndef PLAYER_H
#define PLAYER_H

#include "MovableEntity.h"
#include <chrono>

class Player : public MovableEntity {
private:
    static constexpr float DEFAULT_SPEED = 4.0f;
    static constexpr float DEFAULT_SNAP_DISTANCE = 0.05f;
    static const MoveDir DEFAULT_MOVE_DIR = MoveDir::NONE;
    static constexpr bool DEFAULT_DIR_CHANGE_REQUEST_EXPIRE = true;
    static constexpr float DEFAULT_DIR_CHANGE_REQUEST_EXPIRE_AFTER_MS = 1000;
    static constexpr int BLINK_DURATION_MS = 500;
    bool invincibleBlink = false;
    uint64_t nextBlinkTimeMs = 0;
    bool isInvincible = false;
    uint64_t invincibleEndTimeMs = 0;
    uint64_t getTimeMs();
    float playerBodyColorRed = 1.0f;
    float playerBodyColorGreen = 1.0f;
    float playerBodyColorBlue = 0.0f;

public:
    Player();
    Player(const Player& other);
    Player(Map* map, Point3D playerOrigin, BoundingBox3D playerBoundingBox);
    virtual void render();
    void move(MoveDir requestedMoveDir, bool& isNewRequest, float frameTimeMs);
    void update(int& collectedPellets);
    void setIsInvincible(uint64_t durationMs);
    bool getIsInvincible() const { return isInvincible; }
};

#endif // PLAYER_H
