#include "gl_includes.h"
#include "GameControl.h"
#include "Game.h"
#include "MoveDir.h"
#include "MapFactory.h"
#include <algorithm>
#include <cmath>
#include <iostream>

const CameraState GameControl::DEFAULT_CAMERA_STATE = {
    0.0f, 45.0f, 50.0f,  // yaw, pitch, distance
    0.0f, 0.0f, 0.0f    // lookAtX, lookAtY, lookAtZ
};

GameControl::GameControl() {
    updateGluFromState();
}

void GameControl::update(float frametimeS) {
    handleWasdMovement();
    handleCameraOrbitting();
    handleCameraPosMoving();
    handleCameraZooming();
    if (!autoCamera) { return; }
    if (autoCameraTargetReached) { return; }
    updateAutoCameraTransition(frametimeS);
}

void GameControl::handleWasdMovement() {
    // WASD movement
    if (isKeyFlagPressed('w')) { movementChanged = true; this->moveDir = MoveDir::FWD; }
    if (isKeyFlagPressed('s')) { movementChanged = true; this->moveDir = MoveDir::BWD; }
    if (isKeyFlagPressed('a')) { movementChanged = true; this->moveDir = MoveDir::LEFT; }
    if (isKeyFlagPressed('d')) { movementChanged = true; this->moveDir = MoveDir::RIGHT; }
}

void GameControl::updateAutoCameraTransition(float frameTimeS) {
    float frameTimeNormalizedSpeed = frametimeNormalizedTransitionSpeed(frameTimeS);

    auto lerp = [](float a, float b, float t) {
        return a + (b - a) * t;
        };

    auto easeInOut = [](float t) {
        return t * t * (3 - 2 * t); // smoothstep
        };

    float speedFactor = std::min(frameTimeNormalizedSpeed, MapFactory::TILE_SIZE / 2.0f);
    float easedSpeedFactor = easeInOut(speedFactor);

    cameraState.yaw = lerp(cameraState.yaw, autoCameraTarget.yaw, easedSpeedFactor);
    cameraState.pitch = lerp(cameraState.pitch, autoCameraTarget.pitch, easedSpeedFactor);
    cameraState.distance = lerp(cameraState.distance, autoCameraTarget.distance, easedSpeedFactor);

    cameraState.lookAtX = lerp(cameraState.lookAtX, autoCameraTarget.lookAtX, easedSpeedFactor);
    cameraState.lookAtY = lerp(cameraState.lookAtY, autoCameraTarget.lookAtY, easedSpeedFactor);
    cameraState.lookAtZ = lerp(cameraState.lookAtZ, autoCameraTarget.lookAtZ, easedSpeedFactor);

    updateGluFromState();

    if (isAutoCameraAtTarget()) {
        autoCameraTargetReached = true;
        cameraState = autoCameraTarget;
        updateGluFromState();
    }
}

// Handles camera look at point moving
void GameControl::handleCameraPosMoving() {
    if (isButtonPressed(GLUT_LEFT_BUTTON)) {
        if (isOrbitting) { return; }

        if (!isMovingPos) {
            resetMouseDelta();
            isMovingPos = true;
            return;
        }

        // Get yaw in radians
        float yawRad = cameraState.yaw * GameControl::DEG_TO_RAD;

        // Rotate mouse deltas into world space
        float deltaX = (mouseXDelta * cosf(yawRad) - mouseYDelta * sinf(yawRad)) * movingPosSensitivity;
        float deltaZ = (mouseXDelta * sinf(yawRad) + mouseYDelta * cosf(yawRad)) * movingPosSensitivity;

        // Calculate new tentative lookAt position
        float newLookAtX = cameraGlu.lookAtX - deltaX;
        float newLookAtZ = cameraGlu.lookAtZ - deltaZ;

        // Get map corner points
        Map* map = Game::getInstance().getMap();
        MapCornerPoints cornerPoints = map->getMapCornerPoints();

        // Check if the new lookAt is within bounds
        bool inBounds = (
            newLookAtX >= cornerPoints.lowerLeft.x &&
            newLookAtX <= cornerPoints.lowerRight.x &&
            newLookAtZ >= cornerPoints.lowerLeft.z &&
            newLookAtZ <= cornerPoints.upperLeft.z
            );

        // Only apply if within bounds
        if (inBounds) {
            cameraGlu.lookAtX = newLookAtX;
            cameraGlu.lookAtZ = newLookAtZ;
            cameraGlu.posX -= deltaX;
            cameraGlu.posZ -= deltaZ;

            updateStateFromGlu();
        }

        resetMouseDelta();
        return;
    }
    isMovingPos = false;
}


// Handles camera orbitting
void GameControl::handleCameraOrbitting() {
    if (isMovingPos) { return; }

    if (isButtonPressed(GLUT_RIGHT_BUTTON)) {
        if (!isOrbitting) {
            isOrbitting = true;
            resetMouseDelta();
            return;
        }

        cameraState.yaw += mouseXDelta * orbittingDegPerPixel;
        cameraState.pitch -= mouseYDelta * orbittingDegPerPixel;

        updateGluFromState();
        resetMouseDelta();
        return;
    }
    isOrbitting = false;
}


// Handles camera zoom
void GameControl::handleCameraZooming() {

    if (isButtonFlagPressed(GLUT_WHEEL_UP)) {
        resetButtonFlagPressed(GLUT_WHEEL_UP);
        float zoomStep = 0.5f;
        cameraState.distance = cameraState.distance + zoomStep;
        updateGluFromState();
    }

    if (isButtonFlagPressed(GLUT_WHEEL_DOWN)) {
        resetButtonFlagPressed(GLUT_WHEEL_DOWN);
        float zoomStep = 0.5f;
        cameraState.distance = cameraState.distance - zoomStep;
        updateGluFromState();
    }
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

void GameControl::updateGluFromState() {
    CameraGlu glu;

    // Convert yaw and pitch to radians
    float yawRad = cameraState.yaw * GameControl::DEG_TO_RAD;
    float pitchRad = cameraState.pitch * GameControl::DEG_TO_RAD;

    // Spherical to Cartesian conversion (camera orbiting around target)
    float camX = cameraState.distance * cosf(pitchRad) * sinf(yawRad);
    float camY = cameraState.distance * sinf(pitchRad);
    float camZ = cameraState.distance * cosf(pitchRad) * cosf(yawRad);

    // Camera position is offset from the target
    glu.posX = cameraState.lookAtX + camX;
    glu.posY = cameraState.lookAtY + camY;
    glu.posZ = cameraState.lookAtZ + camZ;

    // Look-at point is the target itself
    glu.lookAtX = cameraState.lookAtX;
    glu.lookAtY = cameraState.lookAtY;
    glu.lookAtZ = cameraState.lookAtZ;

    // World up vector (you could improve this if you're near poles)
    glu.upX = 0.0f;
    glu.upY = 1.0f;
    glu.upZ = 0.0f;

    cameraGlu = glu;
}

void GameControl::updateStateFromGlu() {
    // Vector from lookAt to camera position
    float dx = cameraGlu.posX - cameraGlu.lookAtX;
    float dy = cameraGlu.posY - cameraGlu.lookAtY;
    float dz = cameraGlu.posZ - cameraGlu.lookAtZ;

    // Calculate distance
    float distance = std::sqrt(dx * dx + dy * dy + dz * dz);

    // Prevent division by zero
    if (distance == 0.0f) return;

    // Calculate pitch and yaw in radians
    float pitchRad = std::asin(dy / distance);
    float yawRad = std::atan2(dx, dz);

    // Convert radians to degrees
    float pitch = pitchRad * GameControl::RAD_TO_DEG;
    float yaw = yawRad * GameControl::RAD_TO_DEG;

    // Update cameraState
    cameraState.pitch = pitch;
    cameraState.yaw = yaw;
    cameraState.distance = distance;

    cameraState.lookAtX = cameraGlu.lookAtX;
    cameraState.lookAtY = cameraGlu.lookAtY;
    cameraState.lookAtZ = cameraGlu.lookAtZ;
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