#include "gl_includes.h"
#include "GameCamera.h"
#include "Game.h"
#include "MoveDir.h"
#include "MapFactory.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include "GameUserInput.h"

GameCamera::GameCamera() {
    setCameraMode(CameraMode::InteractiveMapView);
    updateGluFromState();
}

void GameCamera::setCameraMode(CameraMode cameraMode) {
    this->cameraMode = cameraMode;
    if (cameraMode == CameraMode::FollowingPlayer || 
        cameraMode == CameraMode::InteractiveMapView) { enableAutoCamera(); }
}

void GameCamera::setNextCameraMode() {
    switch (cameraMode) {
    case CameraMode::Free:
        setCameraMode(CameraMode::InteractiveMapView);
        break;
    case CameraMode::InteractiveMapView:
        setCameraMode(CameraMode::FollowingPlayer);
        break;
    case CameraMode::FollowingPlayer:
        setCameraMode(CameraMode::Free);
        break;
    }
}

void GameCamera::update(float frametimeS) {
    Game* game = &Game::getInstance();

    // Switch to free camera if user moved the camera pos
    if (cameraMode == CameraMode::FollowingPlayer && !autoCameraMoving) {
        cameraMode = CameraMode::Free;
    }
    
    if (cameraMode == CameraMode::FollowingPlayer) {
        updateFollowingPlayerTarget();
    }

    if (cameraMode == CameraMode::InteractiveMapView) {
        updateInteractiveMapViewTarget();
    }

    handleCameraOrbitting();
    handleCameraPosMoving();
    handleCameraZooming();
    if (!autoCameraMoving && !autoCameraOrbitting && !autoCameraZooming) { return; }
    if (autoCameraTargetReached) { return; }
    updateAutoCameraTransition(frametimeS);
}

void GameCamera::updateFollowingPlayerTarget() {
    Game* game = &Game::getInstance();
    CameraState target = getCameraState();
    Point3D playerCenter = game->getPlayer()->getAbsoluteCenterPoint();
    target.lookAtX = playerCenter.x;
    target.lookAtY = playerCenter.y;
    target.lookAtZ = playerCenter.z;
    setNewCameraTarget(target);
}


// Static helper functions for InteractiveMapView adjustments
float GameCamera::computeInteractiveYaw(float centerFracX) {
    constexpr float baseYaw = GameCamera::DEFAULT_CAMERA_STATE.yaw;
    constexpr float maxDelta = GameCamera::IMV_MAX_YAW_DELTA;
    return baseYaw + centerFracX * maxDelta;
}

float GameCamera::computeInteractivePitch(float centerFracZ) {
    const float basePitch = GameCamera::DEFAULT_CAMERA_STATE.pitch;
    if (centerFracZ >= 0.0f) {
        return basePitch + centerFracZ * GameCamera::IMV_MAX_PITCH_DELTA_UP;
    }
    else {
        return basePitch + centerFracZ * GameCamera::IMV_MAX_PITCH_DELTA_DOWN;
    }
}

void GameCamera::updateInteractiveMapViewTarget() {
    // Compute player-relative fractions
    Game* game = &Game::getInstance();
    Point3D pc = game->getPlayer()->getAbsoluteCenterPoint();
    auto cps = game->getMap()->getMapCornerPoints();
    float minX = cps.lowerLeft.x, maxX = cps.upperRight.x;
    float minZ = cps.lowerLeft.z, maxZ = cps.upperRight.z;
    float fracX = (pc.x - minX) / (maxX - minX);
    float fracZ = (pc.z - minZ) / (maxZ - minZ);
    float centerFracX = (fracX - 0.5f) * 2.0f;
    float centerFracZ = (fracZ - 0.5f) * 2.0f;

    // Map center for lookAt
    float cx = (minX + maxX) * 0.5f;
    float cz = (minZ + maxZ) * 0.5f;

    // Build target
    CameraState target = DEFAULT_CAMERA_STATE;
    target.lookAtX = cx;
    target.lookAtY = 0.0f;
    target.lookAtZ = cz;

    // Apply yaw & pitch via static helpers
    target.yaw = computeInteractiveYaw(centerFracX);
    float rawPitch = computeInteractivePitch(centerFracZ);
    target.pitch = std::clamp(rawPitch,
        DEFAULT_CAMERA_STATE.pitch - IMV_MAX_PITCH_DELTA_DOWN,
        DEFAULT_CAMERA_STATE.pitch + IMV_MAX_PITCH_DELTA_UP);

    // Compute distance to fit entire map
    float hx = (maxX - minX) * 0.5f;
    float hz = (maxZ - minZ) * 0.5f;
    float mapRadius = std::sqrt(hx * hx + hz * hz);

    float aspect = static_cast<float>(glutGet(GLUT_WINDOW_WIDTH)) /
        static_cast<float>(glutGet(GLUT_WINDOW_HEIGHT));
    constexpr float FOVY_DEG = 60.0f;
    float fovY_rad = FOVY_DEG * DEG_TO_RAD * 0.5f;
    float fovX_rad = atanf(tanf(fovY_rad) * aspect);
    float distY = mapRadius / sinf(fovY_rad);
    float distX = mapRadius / sinf(fovX_rad);
    target.distance = std::max(distX, distY);

    target.lookAtZ = target.lookAtZ + IMV_Z_OFFSET;

    setNewCameraTarget(target);
}

void GameCamera::updateAutoCameraTransition(float frameTimeS) {
    float frameTimeNormalizedSpeed = frametimeNormalizedTransitionSpeed(frameTimeS);

    auto lerp = [](float a, float b, float t) {
        return a + (b - a) * t;
        };

    auto easeInOut = [](float t) {
        return t * t * (3 - 2 * t); // smoothstep
        };

    float speedFactor = std::min(frameTimeNormalizedSpeed, MapFactory::TILE_SIZE / 2.0f);
    float easedSpeedFactor = easeInOut(speedFactor);

    // Only orbit (yaw, pitch) when autoCameraOrbitting
    if (autoCameraOrbitting) {
        cameraState.yaw = lerp(cameraState.yaw, autoCameraTarget.yaw, easedSpeedFactor);
        cameraState.pitch = lerp(cameraState.pitch, autoCameraTarget.pitch, easedSpeedFactor);
    }

    // Only zoom (distance) when autoCameraZooming
    if (autoCameraZooming) {
        cameraState.distance = lerp(cameraState.distance, autoCameraTarget.distance, easedSpeedFactor);
    }

    // Only move (lookAtX/Y/Z) when autoCameraMoving
    if (autoCameraMoving) {
        cameraState.lookAtX = lerp(cameraState.lookAtX, autoCameraTarget.lookAtX, easedSpeedFactor);
        cameraState.lookAtY = lerp(cameraState.lookAtY, autoCameraTarget.lookAtY, easedSpeedFactor);
        cameraState.lookAtZ = lerp(cameraState.lookAtZ, autoCameraTarget.lookAtZ, easedSpeedFactor);
    }

    updateGluFromState();

    if (cameraState == autoCameraTarget) {
        autoCameraTargetReached = true;
        cameraState = autoCameraTarget;
        updateGluFromState();
    }
}

// Handles camera look-at point moving
void GameCamera::handleCameraPosMoving() {
    GameUserInput& gc = GameUserInput::getInstance();
    if (!gc.isButtonPressed(GLUT_LEFT_BUTTON)) {
        isMovingPos = false;
        return;
    }
    if (isOrbitting) return;

    if (!isMovingPos) {
        enableManualCamera();
        gc.resetMouseDelta();
        isMovingPos = true;
        return;
    }

    float dx = gc.getMouseXDelta() * movingPosSensitivity;
    float dy = -gc.getMouseYDelta() * movingPosSensitivity;

    float yawRad = cameraState.yaw * GameCamera::DEG_TO_RAD;
    float c = cosf(yawRad), s = sinf(yawRad);
    float fwdX = -s;
    float fwdZ = -c;
    float rightX = c;
    float rightZ = -s;

    float panX = rightX * dx + fwdX * dy;
    float panZ = rightZ * dx + fwdZ * dy;

    float newLAx = cameraGlu.lookAtX + panX;
    float newLAz = cameraGlu.lookAtZ + panZ;
    float newPx = cameraGlu.posX + panX;
    float newPz = cameraGlu.posZ + panZ;

    Map* map = Game::getInstance().getMap();
    auto b = map->getMapCornerPoints();
    bool inBounds =
        newLAx >= b.lowerLeft.x && newLAx <= b.lowerRight.x &&
        newLAz >= b.lowerLeft.z && newLAz <= b.upperLeft.z;

    if (inBounds) {
        cameraGlu.lookAtX = newLAx;
        cameraGlu.lookAtZ = newLAz;
        cameraGlu.posX = newPx;
        cameraGlu.posZ = newPz;
        updateStateFromGlu();
    }

    gc.resetMouseDelta();
}

// Handles camera orbitting
void GameCamera::handleCameraOrbitting() {
    GameUserInput& gc = GameUserInput::getInstance();
    if (isMovingPos) { return; }

    if (gc.isButtonPressed(GLUT_RIGHT_BUTTON)) {
        if (!isOrbitting) {
            isOrbitting = true;
            gc.resetMouseDelta();
            return;
        }
        autoCameraOrbitting = false;
        cameraState.yaw += gc.getMouseXDelta() * orbittingDegPerPixel;
        cameraState.pitch -= gc.getMouseYDelta() * orbittingDegPerPixel;

        updateGluFromState();
        gc.resetMouseDelta();
        return;
    }
    isOrbitting = false;
}


// Handles camera zoom
void GameCamera::handleCameraZooming() {
    GameUserInput& gc = GameUserInput::getInstance();
    if (gc.isButtonFlagPressed(GLUT_WHEEL_UP)) {
        gc.resetButtonFlagPressed(GLUT_WHEEL_UP);
        autoCameraZooming = false;
        float zoomStep = 0.5f;
        cameraState.distance = cameraState.distance + zoomStep;
        updateGluFromState();
    }

    if (gc.isButtonFlagPressed(GLUT_WHEEL_DOWN)) {
        gc.resetButtonFlagPressed(GLUT_WHEEL_DOWN);
        autoCameraZooming = false;
        float zoomStep = 0.5f;
        cameraState.distance = cameraState.distance - zoomStep;
        updateGluFromState();
    }
}

void GameCamera::updateGluFromState() {
    CameraGlu glu;

    // Convert yaw and pitch to radians
    float yawRad = cameraState.yaw * GameCamera::DEG_TO_RAD;
    float pitchRad = cameraState.pitch * GameCamera::DEG_TO_RAD;

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

    // if pitch > 90°, we are "upside down" so flip the up vector
    if (cameraState.pitch > 90.0f && cameraState.pitch < 270.0f) {
        glu.upX = 0.0f;
        glu.upY = -1.0f;
        glu.upZ = 0.0f;
    }
    else {
        glu.upX = 0.0f;
        glu.upY = 1.0f;
        glu.upZ = 0.0f;
    }

    cameraGlu = glu;
}

void GameCamera::updateStateFromGlu() {
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
    float pitch = pitchRad * GameCamera::RAD_TO_DEG;
    float yaw = yawRad * GameCamera::RAD_TO_DEG;

    // Update cameraState
    cameraState.pitch = pitch;
    cameraState.yaw = yaw;
    cameraState.distance = distance;

    cameraState.lookAtX = cameraGlu.lookAtX;
    cameraState.lookAtY = cameraGlu.lookAtY;
    cameraState.lookAtZ = cameraGlu.lookAtZ;
}