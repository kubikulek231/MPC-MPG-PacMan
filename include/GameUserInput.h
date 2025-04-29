#ifndef GameUserInput_H
#define GameUserInput_H
#include "gl_includes.h"
#include <unordered_set>
#include <unordered_map>
#include "CameraModels.h"
#include "MoveDir.h"
#include "MapFactory.h"
#include "GameUserInput.h"
#include "GameCamera.h"

#define GLUT_WHEEL_DOWN 3
#define GLUT_WHEEL_UP 4

class GameUserInput {
public:
    std::unordered_set<unsigned char> trackedKeyboardKeys = { 'w', 'a', 's', 'd', 'x', 'y', 'c', '\x1B'};
    std::unordered_set<int> trackedMouseButtons = { GLUT_LEFT_BUTTON, 
                                                    GLUT_RIGHT_BUTTON, 
                                                    GLUT_MIDDLE_BUTTON, 
                                                    GLUT_WHEEL_DOWN,
                                                    GLUT_WHEEL_UP };

    static GameUserInput& getInstance() {
        static GameUserInput instance;
        return instance;
    }

    void mouseButton(int button, int state, int x, int y);
    void mouseMotion(int x, int y);
    void keyboard(unsigned char key, int x, int y);
    void keyboardUp(unsigned char key, int x, int y);

    bool isKeyPressed(unsigned char key) const;
    bool isKeyReleased(unsigned char key) const;

    bool isButtonPressed(int key) const;
    bool isButtonReleased(int key) const;

    bool isKeyFlagPressed(unsigned char key) const;
    bool isButtonFlagPressed(int key) const;

    void resetKeyFlagPressed(unsigned char key);
    void resetButtonFlagPressed(int key);

    bool isKeyFlagPressedAndReleased(unsigned char key) const;
    bool isButtonFlagPressedAndReleased(int key) const;

    void resetKeyFlagPressedAndReleased(unsigned char key);
    void resetButtonFlagPressedAndReleased(int key);

    bool& getMovementChanged() { return movementChanged; }
    bool resetMovementChanged() { movementChanged = false; }
    void setMovementChanged() { movementChanged = true; }

    void setMoveDir(MoveDir moveDir) { this->moveDir = moveDir; }
    MoveDir getMoveDir() const { return moveDir; }
    void resetMouseDelta();

    float getMouseX() const { return mouseX; }
    float getMouseY() const { return mouseY; }

    float getMouseXDelta() const { return mouseXDelta; }
    float getMouseYDelta() const { return mouseYDelta; }

private:
    GameUserInput() = default;
    GameUserInput(const GameUserInput&) = delete;
    GameUserInput& operator=(const GameUserInput&) = delete;

    std::unordered_map<unsigned char, bool> pressedKeys;
    std::unordered_map<unsigned char, bool> lastPressedKeys;

    std::unordered_map<unsigned int, bool> pressedButtons;
    std::unordered_map<unsigned int, bool> lastPressedButtons;

    std::unordered_map<unsigned char, bool> pressedFlagKeys;
    std::unordered_map<unsigned char, bool> pressedFlagButtons;

    std::unordered_map<unsigned char, bool> pressedAndReleasedFlagKeys;
    std::unordered_map<unsigned char, bool> pressedAndReleasedFlagButtons;

    float mouseX = 0;
    float mouseY = 0;

    float lastMouseX = 0;
    float lastMouseY = 0;

    float mouseXDelta = 0;
    float mouseYDelta = 0;

    bool movementChanged = false;
    MoveDir moveDir = MoveDir::NONE;
};

#endif
