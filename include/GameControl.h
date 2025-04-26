#ifndef GAMECONTROL_H
#define GAMECONTROL_H
#include "gl_includes.h"
#include <unordered_set>
#include <unordered_map>
#include "CameraStruct.h"
#include "MoveDir.h"
#include "MapFactory.h"

#define GLUT_WHEEL_DOWN 3
#define GLUT_WHEEL_UP 4

class GameControl {
public:
    static constexpr float PI = 3.14159265358979323846f;
    static constexpr float DEFAULT_MOUSE_SENSITIVITY = 0.08f;
    static constexpr float DEFAULT_ORBITTING_DEG_PER_PIXEL = 0.2f;
    static const CameraState DEFAULT_CAMERA_STATE;
    static constexpr float DEG_TO_RAD = PI / 180.0f;
    static constexpr float RAD_TO_DEG = 180.0f / PI;
    static constexpr float DEFAULT_CAMERA_TRANSITION_SPEED = 7.0f;

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

    // FrametimeMs is needed for auto camera transition timing
    void update(float frameTimeMs);

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
    CameraGlu getCameraGLU() const { return cameraGlu; }
    CameraState getCameraState() const {return cameraState; }

    float getMovingPosSensitivity() const { return movingPosSensitivity; }
    float getOrbittingDegPerPixel() const { return orbittingDegPerPixel; }

    void setMovingPosSensitivity(float newSensitivity) { movingPosSensitivity = newSensitivity; }
    void setOrbittingDegPerPixel(float newDegPerPixel) { orbittingDegPerPixel = newDegPerPixel; }

    bool& getMovementChanged() { return movementChanged; }
    bool resetMovementChanged() { movementChanged = false; }

    MoveDir getMoveDir() const { return moveDir; }

    void enableManualCamera() { autoCamera = false; }
    void enableAutoCamera() { autoCamera = true; }

    void setNewCameraTarget(CameraState targetState) {
        if (targetState == cameraState) { return; }
        autoCameraTarget = targetState;
        autoCameraTargetReached = false;
    }

    bool isAutoCameraAtTarget() const { return autoCameraTargetReached; }

private:
    GameControl();
    GameControl(const GameControl&) = delete;
    GameControl& operator=(const GameControl&) = delete;

    void updateGluFromState();
    void updateStateFromGlu();

    void handleWasdMovement();
    void handleCameraOrbitting();
    void handleCameraPosMoving();
    void handleCameraZooming();

    float frametimeNormalizedTransitionSpeed(float frametimeS) { return frametimeS * DEFAULT_CAMERA_TRANSITION_SPEED * MapFactory::TILE_SIZE; }
    void updateAutoCameraTransition(float frametimeS);

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
    CameraGlu cameraGlu;

    float movingPosSensitivity = DEFAULT_MOUSE_SENSITIVITY;
    float orbittingDegPerPixel = DEFAULT_ORBITTING_DEG_PER_PIXEL;

    bool isOrbitting = false;
    bool isMovingPos = false;

    bool movementChanged = false;

    MoveDir moveDir = MoveDir::NONE;

    bool autoCamera = false;

    // AutoCamera target
    bool autoCameraTargetReached = false;
    CameraState autoCameraTarget;
};

#endif
