#include "gl_includes.h"
#include "GameCamera.h"
#include "Game.h"
#include "MoveDir.h"
#include "MapFactory.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include "GameControl.h"

const CameraState GameCamera::DEFAULT_CAMERA_STATE = {
    0.0f, 75.0f, 35.0f,  // yaw, pitch, distance
    0.0f, 0.0f, 0.0f    // lookAtX, lookAtY, lookAtZ
};

GameCamera::GameCamera() {
    setCameraType(CameraType::InteractiveMapView);
    updateGluFromState();
}

void GameCamera::setCameraType(CameraType cameraType) {
    this->cameraType = cameraType;
    if (cameraType == CameraType::FollowingPlayer || 
        cameraType == CameraType::InteractiveMapView) { enableAutoCamera(); }
}

void GameCamera::update(float frametimeS) {
    Game* game = &Game::getInstance();

    // Switch to free camera if user moved the camera pos
    if (cameraType == CameraType::FollowingPlayer && !autoCameraMoving) {
        cameraType = CameraType::Free;
    }
    
    if (cameraType == CameraType::FollowingPlayer) {
        CameraState target = getCameraState();
        Point3D playerCenter = game->getPlayer()->getAbsoluteCenterPoint();
        target.lookAtX = playerCenter.x;
        target.lookAtY = playerCenter.y;
        target.lookAtZ = playerCenter.z;
        setNewCameraTarget(target);
    }

    if (cameraType == CameraType::InteractiveMapView) {
        // 1) figure out where the player is on the map (-1…+1)
        Game* game = &Game::getInstance();
        Point3D pc = game->getPlayer()->getAbsoluteCenterPoint();
        auto cps = game->getMap()->getMapCornerPoints();
        float minX = cps.lowerLeft.x, maxX = cps.upperRight.x;
        float minZ = cps.lowerLeft.z, maxZ = cps.upperRight.z;
        float fracX = (pc.x - minX) / (maxX - minX);
        float fracZ = (pc.z - minZ) / (maxZ - minZ);
        float centerFracX = (fracX - 0.5f) * 2.0f;
        float centerFracZ = (fracZ - 0.5f) * 2.0f;

        // 2) map center for lookAt
        float cx = (minX + maxX) * 0.5f;
        float cz = (minZ + maxZ) * 0.5f;

        // 3) start from defaults
        CameraState target = DEFAULT_CAMERA_STATE;
        target.lookAtX = cx;
        target.lookAtY = 0.0f;
        target.lookAtZ = cz;

        // centerFracZ (–1 at bottom -> +1 at top)
        float dynamicPitch;
        if (centerFracZ >= 0.0f) {
            dynamicPitch = DEFAULT_CAMERA_STATE.pitch
                + centerFracZ * MAX_PITCH_DELTA_UP;
        }
        else {
            dynamicPitch = DEFAULT_CAMERA_STATE.pitch
                + centerFracZ * MAX_PITCH_DELTA_DOWN;
        }

        // now clamp into the full range you allow:
        // lowest possible: DEFAULT_PITCH – PITCH_DELTA_DOWN
        // highest possible: DEFAULT_PITCH + PITCH_DELTA_UP
        target.pitch = std::clamp(
            dynamicPitch,
            DEFAULT_CAMERA_STATE.pitch - MAX_PITCH_DELTA_DOWN,
            DEFAULT_CAMERA_STATE.pitch + MAX_PITCH_DELTA_UP
        );

        // 5) dynamically compute distance so the entire map fits
        float hx = (maxX - minX) * 0.5f;
        float hz = (maxZ - minZ) * 0.5f;
        float mapRadius = std::sqrt(hx * hx + hz * hz);

        // get your window aspect
        float screenW = static_cast<float>(glutGet(GLUT_WINDOW_WIDTH));
        float screenH = static_cast<float>(glutGet(GLUT_WINDOW_HEIGHT));
        float aspect = screenW / screenH;

        target.yaw = DEFAULT_CAMERA_STATE.yaw + centerFracX * MAX_YAW_DELTA;

        // choose your vertical FOV (must match what you use in glPerspective)
        constexpr float DEFAULT_FOV_Y_DEG = 60.0f;
        const   float DEFAULT_FOV_Y_RAD = DEFAULT_FOV_Y_DEG * GameCamera::DEG_TO_RAD;

        // what half-FOV is in X?
        float halfFovY = DEFAULT_FOV_Y_RAD * 0.5f;
        float halfFovX = atanf(tanf(halfFovY) * aspect);

        // distance needed so a circle of radius mapRadius just fits:
        float distY = mapRadius / sinf(halfFovY);
        float distX = mapRadius / sinf(halfFovX);

        target.distance = std::max(distX, distY);

        // 6) hand off to your smoothing / lerp system
        setNewCameraTarget(target);
    }

    handleCameraOrbitting();
    handleCameraPosMoving();
    handleCameraZooming();
    if (!autoCameraMoving && !autoCameraOrbitting && !autoCameraZooming) { return; }
    if (autoCameraTargetReached) { return; }
    updateAutoCameraTransition(frametimeS);
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
    GameControl& gc = GameControl::getInstance();
    if (!gc.isButtonPressed(GLUT_LEFT_BUTTON)) {
        isMovingPos = false;
        return;
    }
    if (isOrbitting) return;

    if (!isMovingPos) {
        autoCameraMoving = false;
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
    GameControl& gc = GameControl::getInstance();
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
    GameControl& gc = GameControl::getInstance();
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