#include "gl_includes.h"
#include "GameUserInput.h"
#include "Game.h"
#include "MoveDir.h"
#include "MapFactory.h"
#include <algorithm>
#include <cmath>
#include <iostream>

// Glut callback for registering keyboard down events
void GameUserInput::keyboard(unsigned char key, int x, int y) {
    if (trackedKeyboardKeys.find(key) != trackedKeyboardKeys.end()) {
        pressedKeys[key] = true;
        lastPressedKeys[key] = false;

        pressedFlagKeys[key] = true;
    }
}

// Glut callback for registering keyboard up events
void GameUserInput::keyboardUp(unsigned char key, int x, int y) {
    if (trackedKeyboardKeys.find(key) != trackedKeyboardKeys.end()) {
        pressedKeys[key] = false;
        lastPressedKeys[key] = true;
        pressedAndReleasedFlagKeys[key] = pressedFlagKeys[key];
        pressedFlagKeys[key] = false;
    }
}

// Glut callback for registering mouse button inputs
void GameUserInput::mouseButton(int button, int state, int x, int y) {
    if (trackedMouseButtons.find(button) == trackedMouseButtons.end()) { return; }

    // Handle scroll wheel as one-time impulse
    if (button == GLUT_WHEEL_UP || button == GLUT_WHEEL_DOWN) {
        pressedFlagButtons[button] = true;
        pressedAndReleasedFlagButtons[button] = true;
        return;
    }
    if (state == GLUT_DOWN) {
        pressedButtons[button] = true;
        lastPressedButtons[button] = false;
        pressedFlagButtons[button] = true;
    }
    else if (state == GLUT_UP) {
        pressedButtons[button] = false;
        lastPressedButtons[button] = true;
        pressedAndReleasedFlagButtons[button] = pressedFlagButtons[button];
        pressedFlagButtons[button] = false;
    }
}

bool GameUserInput::isKeyPressed(unsigned char key) const {
    return pressedKeys.count(key) && pressedKeys.at(key);
}

bool GameUserInput::isKeyReleased(unsigned char key) const {
    return lastPressedKeys.count(key) && lastPressedKeys.at(key);
}

bool GameUserInput::isKeyFlagPressed(unsigned char key) const {
    return pressedFlagKeys.count(key) && pressedFlagKeys.at(key);
}

bool GameUserInput::isKeyFlagPressedAndReleased(unsigned char key) const {
    return pressedAndReleasedFlagKeys.count(key) && pressedAndReleasedFlagKeys.at(key);
}

void GameUserInput::resetKeyFlagPressed(unsigned char key) {
    pressedFlagKeys[key] = false;
}

void GameUserInput::resetKeyFlagPressedAndReleased(unsigned char key) {
    pressedAndReleasedFlagKeys[key] = false;
}

bool GameUserInput::isButtonPressed(int button) const {
    return pressedButtons.count(button) && pressedButtons.at(button);
}

bool GameUserInput::isButtonReleased(int button) const {
    return lastPressedButtons.count(button) && lastPressedButtons.at(button);
}

bool GameUserInput::isButtonFlagPressed(int button) const {
    return pressedFlagButtons.count(button) && pressedFlagButtons.at(button);
}

bool GameUserInput::isButtonFlagPressedAndReleased(int button) const {
    return pressedAndReleasedFlagButtons.count(button) && pressedAndReleasedFlagButtons.at(button);
}

void GameUserInput::resetButtonFlagPressed(int button) {
    pressedFlagButtons[button] = false;
}

void GameUserInput::resetButtonFlagPressedAndReleased(int button) {
    pressedAndReleasedFlagButtons[button] = false;
}

void GameUserInput::resetMouseDelta() {
    lastMouseX = mouseX;
    lastMouseY = mouseY;
    mouseXDelta = 0;
    mouseYDelta = 0;
}

void GameUserInput::mouseMotion(int x, int y) {
    lastMouseX = mouseX;
    lastMouseY = mouseY;
    mouseX = x;
    mouseY = y;
    mouseXDelta = mouseX - lastMouseX;
    mouseYDelta = mouseY - lastMouseY;
}