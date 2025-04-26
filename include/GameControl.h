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

private:
    GameControl() = default;
    GameControl(const GameControl&) = delete;
    GameControl& operator=(const GameControl&) = delete;
    void handleWasdMovement();
    void changeCameraMode();
};

#endif
