#ifndef GAMECONTROL_H
#define GAMECONTROL_H
#include "gl_includes.h"
#include <unordered_set>
#include <unordered_map>

#define GLUT_WHEEL_DOWN 3
#define GLUT_WHEEL_UP 4

class Game; // Forward declaration to avoid circular dependency

class GameControl {
public:
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
    void setKeyFlagPressed(unsigned char key, bool setState);
    void setButtonFlagPressed(int key, bool setState);

private:
    GameControl() = default;
    GameControl(const GameControl&) = delete;
    GameControl& operator=(const GameControl&) = delete;

    std::unordered_map<unsigned char, bool> pressedKeys;
    std::unordered_map<unsigned char, bool> lastPressedKeys;

    std::unordered_map<unsigned char, bool> pressedButtons;
    std::unordered_map<unsigned char, bool> lastPressedButtons;

    std::unordered_map<unsigned char, bool> pressedFlagKeys;
    std::unordered_map<unsigned char, bool> pressedFlagButtons;

    float mouseX = 0;
    float mouseY = 0;
    float lastMouseX = 0;
    float lastMouseY = 0;
};

#endif
