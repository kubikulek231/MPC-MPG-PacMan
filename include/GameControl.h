#ifndef GAMECONTROL_H
#define GAMECONTROL_H
#include "gl_includes.h"
#include <unordered_set>
#include <unordered_map>
#include "MoveDir.h"

#define GLUT_WHEEL_DOWN 3
#define GLUT_WHEEL_UP 4

struct CameraState {
    float yaw = 0;
    float pitch = 0;
    float distance = 0;
    float lookAtX = 0;
    float lookAtY = 0;
    float lookAtZ = 0;
};

struct CameraGLU {
    float posX = 0;
    float posY = 0;
    float posZ = 0;
    float upX = 0;
    float upY = 0;
    float upZ = 0;
    float lookAtX = 0;
    float lookAtY = 0;
    float lookAtZ = 0;
};

class GameControl {
public:
    static constexpr float PI = 3.14159265358979323846f;
    static constexpr float MOUSE_SENSITIVITY = 0.005f;
    static constexpr float ORBITTING_DEG_PER_PIXEL = 0.15f;
    static const CameraState DEFAULT_CAMERA_STATE;
    static constexpr float DEG_TO_RAD = PI / 180.0f;
    static constexpr float RAD_TO_DEG = 180.0f / PI;

    std::unordered_set<unsigned char> trackedKeyboardKeys = { 'w', 'a', 's', 'd', 'x', 'y' };
    std::unordered_set<int> trackedMouseButtons = { GLUT_LEFT_BUTTON, 
                                                    GLUT_RIGHT_BUTTON, 
                                                    GLUT_MIDDLE_BUTTON, 
                                                    GLUT_WHEEL_DOWN,
                                                    GLUT_WHEEL_UP };

    static GameControl& getInstance() {
        static GameControl instance;
        return instance;
    }

    void update();

    void mouseButton(int button, int state, int x, int y);
    void mouseMotion(int x, int y);
    void keyboard(unsigned char key, int x, int y);
    void keyboardUp(unsigned char key, int x, int y);

    bool isKeyPressedAndReleased(unsigned char key);
    bool isKeyPressed(unsigned char key);
    bool isKeyReleased(unsigned char key);

    bool isButtonPressedAndReleased(int key);
    bool isButtonPressed(int key);
    bool isButtonReleased(int key);

    bool isKeyFlagPressed(unsigned char key);
    bool isButtonFlagPressed(int key);

    void resetKeyFlagPressed(unsigned char key);
    void resetButtonFlagPressed(int key);

    void setCameraState(CameraState newCameraState) { cameraState = newCameraState; }
    CameraGLU getCameraGLU() const { return cameraGlu; }

    bool& getMovementChanged() { return movementChanged; }
    bool resetMovementChanged() { movementChanged = false; }

    MoveDir getMoveDir() const { return moveDir; }

private:
    GameControl();
    GameControl(const GameControl&) = delete;
    GameControl& operator=(const GameControl&) = delete;

    void updateGluFromState();
    void updateStateFromGlu();

    void handleWasdMovement();
    void handleCameraOrbitting();
    void handleCameraLookAtMoving();
    void handleCameraZooming();

    void resetMouseDelta();

    std::unordered_map<unsigned char, bool> pressedKeys;
    std::unordered_map<unsigned char, bool> lastPressedKeys;

    std::unordered_map<unsigned int, bool> pressedButtons;
    std::unordered_map<unsigned int, bool> lastPressedButtons;

    std::unordered_map<unsigned char, bool> pressedFlagKeys;
    std::unordered_map<unsigned char, bool> pressedFlagButtons;

    float mouseX = 0;
    float mouseY = 0;

    float lastMouseX = 0;
    float lastMouseY = 0;

    float mouseXDelta = 0;
    float mouseYDelta = 0;

    CameraState cameraState = DEFAULT_CAMERA_STATE;
    CameraGLU cameraGlu;

    bool isOrbitting = false;
    bool isLookAtMoving = false;

    bool movementChanged = false;

    MoveDir moveDir = MoveDir::NONE;
};

#endif
