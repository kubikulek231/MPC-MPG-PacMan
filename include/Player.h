#ifndef PLAYER_H
#define PLAYER_H

#include "gl_includes.h"
#include "MovableEntity.h"
#include <chrono>
#include "SpeedoMeter.h"

class Player : public MovableEntity {
private:
    bool invincibleBlink = false;
    uint64_t nextBlinkTimeMs = 0;
    bool isInvincible = false;
    uint64_t invincibleEndTimeMs = 0;
    uint64_t getTimeMs();
    float playerBodyColorRed = 1.0f;
    float playerBodyColorGreen = 1.0f;
    float playerBodyColorBlue = 0.0f;
    
    float playerAnimationState = 0.0f;
    bool playerMouthOpening = true;
    bool playerMouthClosing = false;

    Point3D lastOrigin;
    SpeedoMeter speedoMeter = SpeedoMeter(20);

    uint64_t blinkDurationMs;
    uint64_t invincibleEndTimeAfterMs;
    void animate(float frameTimeMs, bool keepAnimating);
public:
    static constexpr float DEFAULT_SPEED = 4.0f;
    static constexpr float DEFAULT_SNAP_DISTANCE = 0.05f;
    static const MoveDir DEFAULT_MOVE_DIR = MoveDir::NONE;
    static constexpr bool DEFAULT_DIR_CHANGE_REQUEST_EXPIRE = true;
    static constexpr float DEFAULT_DIR_CHANGE_REQUEST_EXPIRE_AFTER_MS = 1000;
    static constexpr int DEFAULT_BLINK_DURATION_MS = 500;
    static constexpr int DEFAULT_INVINCIBLE_EXPIRE_AFTER_MS = 5000;
    Player();
    Player(const Player& other);
    Player(Map* map, Point3D playerOrigin, BoundingBox3D playerBoundingBox);
    virtual void render();
    void move(MoveDir requestedMoveDir, bool& isNewRequest, float frameTimeMs);
    void update(int& collectedPellets);
    void setIsInvincible();
    bool getIsInvincible() const { return isInvincible; }

    uint64_t getBlinkDuration() const { return blinkDurationMs; }
    uint64_t getDirChangeRequestExpireAfterMs() const { return dirChangeRequestExpireAfterMs; }
    uint64_t getInvincibleEndTimeAfterMs() const { return invincibleEndTimeAfterMs; }
    void setBlinkDuration(uint64_t duration) { this->blinkDurationMs = duration; }
    void setInvincibleExpireAfterMs(uint64_t expireAfter) { this->invincibleEndTimeAfterMs = expireAfter; }
};

#endif // PLAYER_H
