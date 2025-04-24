#ifndef GAMECAMERACONTROL_H
#define GAMECAMERACONTROL_H
#include "gl_includes.h"

#define GLUT_WHEEL_DOWN 3
#define GLUT_WHEEL_UP 4

enum class CameraMovementType {
    NONE = 0,
    ORBITTING = 1,
    PANNING = 2,
};


class GameCameraControl {
public:
    static GameCameraControl& getInstance() {
        static GameCameraControl instance;
        return instance;
    }

    void update();

private:
    float mouseX = 0;
    float mouseY = 0;
    float lastMouseX = 0;
    float lastMouseY = 0;
};

#endif
