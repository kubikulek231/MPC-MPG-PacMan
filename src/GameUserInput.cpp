#include "gl_includes.h"
#include "GameUserInput.h"
#include "Game.h"
#include "MoveDir.h"
#include "MapFactory.h"
#include <algorithm>
#include <cmath>
#include <iostream>

void GameUserInput::update() {
    handleWasdMovement();
}

void GameUserInput::handleWasdMovement() {
    // WASD movement
    if (isKeyFlagPressed('w')) { movementChanged = true; this->moveDir = MoveDir::FWD; }
    if (isKeyFlagPressed('s')) { movementChanged = true; this->moveDir = MoveDir::BWD; }
    if (isKeyFlagPressed('a')) { movementChanged = true; this->moveDir = MoveDir::LEFT; }
    if (isKeyFlagPressed('d')) { movementChanged = true; this->moveDir = MoveDir::RIGHT; }
}

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

        pressedFlagKeys[key] = false;
    }
}

// Glut callback for registering mouse button inputs
void GameUserInput::mouseButton(int button, int state, int x, int y) {
    if (trackedMouseButtons.find(button) == trackedMouseButtons.end()) { return; }

    // Handle scroll wheel as one-time impulse
    if (button == GLUT_WHEEL_UP || button == GLUT_WHEEL_DOWN) {
        pressedFlagButtons[button] = true;
        return;
    }

    if (state == GLUT_DOWN) {
        pressedButtons[button] = true;
        lastPressedButtons[button] = false;
        pressedFlagButtons[button] = true;
        return;
    }

    if (state == GLUT_UP) {
        // Reset button state when released
        pressedButtons[button] = false;
        lastPressedButtons[button] = true;
        pressedFlagButtons[button] = false;
        return;
    }
}

// Glut callback for registering mouse position
void GameUserInput::mouseMotion(int x, int y) {
    lastMouseX = mouseX;
    lastMouseY = mouseY;
    mouseX = x;
    mouseY = y;
    mouseXDelta = mouseX - lastMouseX;
    mouseYDelta = mouseY - lastMouseY;
}

void GameUserInput::resetMouseDelta() {
    lastMouseX = mouseX;
    lastMouseY = mouseY;
    mouseXDelta = 0;
    mouseYDelta = 0;
}

bool GameUserInput::isKeyPressed(unsigned char key) {
    return pressedKeys[key];
}

bool GameUserInput::isKeyReleased(unsigned char key) {
    return lastPressedKeys[key];
}

bool GameUserInput::isKeyPressedAndReleased(unsigned char key) {
    return lastPressedKeys[key] && !pressedKeys[key];
}

bool GameUserInput::isButtonPressed(int key) {
    return pressedButtons[key];
}

bool GameUserInput::isButtonReleased(int key) {
    return lastPressedButtons[key];
}

bool GameUserInput::isButtonPressedAndReleased(int key) {
    return lastPressedButtons[key] && !pressedButtons[key];
}

bool GameUserInput::isKeyFlagPressed(unsigned char key) {
    return pressedFlagKeys[key];
}

bool GameUserInput::isButtonFlagPressed(int key) {
    return pressedFlagButtons[key];
}

void GameUserInput::resetKeyFlagPressed(unsigned char key) {
    pressedFlagKeys[key] = false;
}

void GameUserInput::resetButtonFlagPressed(int key) {
    pressedFlagButtons[key] = false;
}