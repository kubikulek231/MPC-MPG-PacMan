/* GameControl::update — WASD unchanged, camera orbit/pan/zoom without GLM */
#include "gl_includes.h"
#include "GameControl.h"
#include "Game.h"
#include "MoveDir.h"
#include <algorithm>
#include <cmath>

#define PI 3.14159265358979323846f
#define MOUSE_SENSITIVITY 0.005f  // tweak this as needed
#define DEG_PER_PIXEL 0.15f // Try values between 0.05 and 0.3

void GameControl::update() {
    GameControl& gc = GameControl::getInstance();
    Game& game = Game::getInstance();

    bool rightPressed = gc.isButtonPressed(GLUT_RIGHT_BUTTON);
    bool leftPressed = gc.isButtonPressed(GLUT_LEFT_BUTTON);

    // Check if we're just starting interaction
    bool rightJustPressed = gc.isButtonFlagPressed(GLUT_RIGHT_BUTTON);
    bool leftJustPressed = gc.isButtonFlagPressed(GLUT_LEFT_BUTTON);

    if ((rightPressed && rightJustPressed) || (leftPressed && leftJustPressed)) {
        // Reset last mouse position to avoid jump on first movement
        gc.lastMouseX = gc.mouseX;
        gc.lastMouseY = gc.mouseY;

        // Clear the flag so it doesn't reset every frame
        gc.setButtonFlagPressed(GLUT_RIGHT_BUTTON, false);
        gc.setButtonFlagPressed(GLUT_LEFT_BUTTON, false);
    }


    // ----- WASD movement remains unchanged -----
    if (gc.isKeyFlagPressed('w')) { game.setIsDirectionKeyPressed(true); game.setMoveDir(MoveDir::FWD); }
    if (gc.isKeyFlagPressed('s')) { game.setIsDirectionKeyPressed(true); game.setMoveDir(MoveDir::BWD); }
    if (gc.isKeyFlagPressed('a')) { game.setIsDirectionKeyPressed(true); game.setMoveDir(MoveDir::LEFT); }
    if (gc.isKeyFlagPressed('d')) { game.setIsDirectionKeyPressed(true); game.setMoveDir(MoveDir::RIGHT); }

    // Compute mouse delta only when a button is pressed
    int dx = 0, dy = 0;
    bool interacting = gc.isButtonPressed(GLUT_LEFT_BUTTON) || gc.isButtonPressed(GLUT_RIGHT_BUTTON);
    if (interacting) {
        dx = gc.mouseX - gc.lastMouseX;
        dy = gc.mouseY - gc.lastMouseY;
    }

    // ----- Camera Orbit (right-click) -----
    if (gc.isButtonPressed(GLUT_RIGHT_BUTTON)) {
        float deltaYaw = dx * DEG_PER_PIXEL;
        float deltaPitch = -dy * DEG_PER_PIXEL;

        float newPitch = game.getCameraAngleX() + deltaPitch;
        newPitch = std::clamp(newPitch, -89.0f, 89.0f);

        game.setCameraAngleY(game.getCameraAngleY() + deltaYaw);
        game.setCameraAngleX(newPitch);
    }

    // ----- Camera Pan (left-click) -----
    if (gc.isButtonPressed(GLUT_LEFT_BUTTON)) {
        float distance = game.getCameraDistance();
        float panSpeed = 0.01f * distance;

        float yawRad = game.getCameraAngleY() * PI / 180.0f;
        float rightX = cosf(yawRad);
        float rightZ = -sinf(yawRad);

        float moveX = -dx * panSpeed * rightX;
        float moveZ = -dx * panSpeed * rightZ;
        float moveY = dy * panSpeed; // vertical pan if desired

        game.setCameraLookAtPosX(game.getCameraLookAtPosX() + moveX);
        game.setCameraLookAtPosZ(game.getCameraLookAtPosZ() + moveZ);
        // Optional Y pan:
        // game.setCameraLookAtPosY(game.getCameraLookAtPosY() + moveY);
    }

    // ----- Zoom (scroll wheel) -----
    if (gc.isButtonFlagPressed(GLUT_WHEEL_UP)) {
        gc.setButtonFlagPressed(GLUT_WHEEL_UP, false);
        float zoomStep = 0.5f;
        game.setCameraDistance(std::clamp(game.getCameraDistance() - zoomStep, 5.0f, 100.0f));
    }
    if (gc.isButtonFlagPressed(GLUT_WHEEL_DOWN)) {
        gc.setButtonFlagPressed(GLUT_WHEEL_DOWN, false);
        float zoomStep = 0.5f;
        game.setCameraDistance(std::clamp(game.getCameraDistance() + zoomStep, 5.0f, 100.0f));
    }
}


void GameControl::keyboard(unsigned char key, int x, int y) {
    if (trackedKeyboardKeys.find(key) != trackedKeyboardKeys.end()) {
        pressedKeys[key] = true;
        lastPressedKeys[key] = false;

        pressedFlagKeys[key] = true;
    }
}

void GameControl::keyboardUp(unsigned char key, int x, int y) {
    if (trackedKeyboardKeys.find(key) != trackedKeyboardKeys.end()) {
        pressedKeys[key] = false;
        lastPressedKeys[key] = true;

        pressedFlagKeys[key] = false;
    }
}

void GameControl::mouseButton(int button, int state, int x, int y) {
    if (trackedMouseButtons.find(button) != trackedMouseButtons.end()) {
        if (state == GLUT_DOWN) {
            pressedButtons[button] = true;
            lastPressedButtons[button] = false;

            pressedFlagButtons[button] = true;
        }
        else if (state == GLUT_UP) {
            // Reset button state when released
            pressedButtons[button] = false;
            lastPressedButtons[button] = true;

            pressedFlagButtons[button] = false;
        }
    }
}


void GameControl::mouseMotion(int x, int y) {
    GameControl& gc = GameControl::getInstance();
    gc.mouseX = x;
    gc.mouseY = y;
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

void GameControl::setKeyFlagPressed(unsigned char key, bool setState) {
    pressedFlagKeys[key] = setState;
}

void GameControl::setButtonFlagPressed(int key, bool setState) {
    pressedFlagButtons[key] = setState;
}