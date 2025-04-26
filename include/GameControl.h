#ifndef GAMECONTROL_H
#define GAMECONTROL_H
#include "gl_includes.h"
#include <unordered_set>
#include <unordered_map>
#include "CameraModels.h"
#include "MoveDir.h"
#include "MapFactory.h"
#include "GameControl.h"
#include "GameCamera.h"

#define GLUT_WHEEL_DOWN 3
#define GLUT_WHEEL_UP 4

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
};

#endif
