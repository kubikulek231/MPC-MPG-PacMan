#ifndef GAMECONTROL_H
#define GAMECONTROL_H
#include "gl_includes.h"
#include <unordered_set>
#include <unordered_map>
#include "CameraModels.h"
#include "MoveDir.h"
#include "MapFactory.h"
#include "GameCamera.h"

class GameControl {
public:
    static GameControl& getInstance() {
        static GameControl instance;
        return instance;
    }
    void update();
    void setMoveDir(MoveDir moveDir) { this->moveDir = moveDir; }
    MoveDir getMoveDir() const { return moveDir; }
    bool& getMovementChanged() { return movementChanged; }
    bool resetMovementChanged() { movementChanged = false; }
    void setMovementChanged() { movementChanged = true; }

    // Disables WASD movement until a full key press-and-release cycle is detected.
    // If a WASD key is already held down when this is called, it must be released,
    // then pressed and released again to enable movement.
    void enableWasdAfterFullPressCycle();

private:
    void handleWasdMovement();
    void handleWasdMovementEnable();
    bool isAnyWasdKeyPressed();
    GameControl() = default;
    GameControl(const GameControl&) = delete;
    GameControl& operator=(const GameControl&) = delete;
    void changeCameraMode();
    MoveDir moveDir = MoveDir::NONE;

    bool waitForKeyReleaseFirst = false;

    bool isWasdMovementEnabled = false;
    bool movementChanged = false;

};

#endif
