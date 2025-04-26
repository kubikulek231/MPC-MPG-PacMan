#include "gl_includes.h"
#include "GameControl.h"
#include "Game.h"
#include "MoveDir.h"
#include "MapFactory.h"
#include <algorithm>
#include <cmath>
#include <iostream>

void GameControl::update() {
    handleWasdMovement();
}

void GameControl::handleWasdMovement() {
    // WASD movement
    if (isKeyFlagPressed('w')) { movementChanged = true; this->moveDir = MoveDir::FWD; }
    if (isKeyFlagPressed('s')) { movementChanged = true; this->moveDir = MoveDir::BWD; }
    if (isKeyFlagPressed('a')) { movementChanged = true; this->moveDir = MoveDir::LEFT; }
    if (isKeyFlagPressed('d')) { movementChanged = true; this->moveDir = MoveDir::RIGHT; }
}

// Glut callback for registering keyboard down events
void GameControl::keyboard(unsigned char key, int x, int y) {
    if (trackedKeyboardKeys.find(key) != trackedKeyboardKeys.end()) {
        pressedKeys[key] = true;
        lastPressedKeys[key] = false;

        pressedFlagKeys[key] = true;
    }
}

// Glut callback for registering keyboard up events
void GameControl::keyboardUp(unsigned char key, int x, int y) {
    if (trackedKeyboardKeys.find(key) != trackedKeyboardKeys.end()) {
        pressedKeys[key] = false;
        lastPressedKeys[key] = true;

        pressedFlagKeys[key] = false;
    }
}

// Glut callback for registering mouse button inputs
void GameControl::mouseButton(int button, int state, int x, int y) {
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
void GameControl::mouseMotion(int x, int y) {
    lastMouseX = mouseX;
    lastMouseY = mouseY;
    mouseX = x;
    mouseY = y;
    mouseXDelta = mouseX - lastMouseX;
    mouseYDelta = mouseY - lastMouseY;
}

void GameControl::resetMouseDelta() {
    lastMouseX = mouseX;
    lastMouseY = mouseY;
    mouseXDelta = 0;
    mouseYDelta = 0;
}

bool GameControl::isKeyPressed(unsigned char key) {
    return pressedKeys[key];
}

bool GameControl::isKeyReleased(unsigned char key) {
    return lastPressedKeys[key];
}

bool GameControl::isKeyPressedAndReleased(unsigned char key) {
    return lastPressedKeys[key] && !pressedKeys[key];
}

bool GameControl::isButtonPressed(int key) {
    return pressedButtons[key];
}

bool GameControl::isButtonReleased(int key) {
    return lastPressedButtons[key];
}

bool GameControl::isButtonPressedAndReleased(int key) {
    return lastPressedButtons[key] && !pressedButtons[key];
}

bool GameControl::isKeyFlagPressed(unsigned char key) {
    return pressedFlagKeys[key];
}

bool GameControl::isButtonFlagPressed(int key) {
    return pressedFlagButtons[key];
}

void GameControl::resetKeyFlagPressed(unsigned char key) {
    pressedFlagKeys[key] = false;
}

void GameControl::resetButtonFlagPressed(int key) {
    pressedFlagButtons[key] = false;
}