#ifndef GAMECAMERA_H
#define GAMECAMERA_H

#include "gl_includes.h"
#include "CameraModels.h"
#include "Player.h"
#include "Game.h"
#include "Map.h"

enum class CameraMode {
    Free = 0,
    InteractiveMapView = 1,
    FollowingPlayer = 2
};

static class GameCamera {
public:
    static constexpr float PI = 3.14159265358979323846f;
    static constexpr float DEFAULT_MOUSE_SENSITIVITY = 0.08f;
    static constexpr float DEFAULT_ORBITTING_DEG_PER_PIXEL = 0.2f;
    static constexpr CameraState DEFAULT_CAMERA_STATE = {
        0.0f, 75.0f, 35.0f,  // yaw, pitch, distance
        0.0f, 0.0f, 0.0f    // lookAtX, lookAtY, lookAtZ
    };
    static constexpr float DEG_TO_RAD = PI / 180.0f;
    static constexpr float RAD_TO_DEG = 180.0f / PI;
    static constexpr float DEFAULT_CAMERA_TRANSITION_SPEED = 7.0f;
    static constexpr float IMV_MAX_PITCH_DELTA_UP = 15.0f;
    static constexpr float IMV_MAX_PITCH_DELTA_DOWN = 8.0f;
    static constexpr float IMV_MAX_YAW_DELTA = 5.0f;
    static constexpr float IMV_Z_OFFSET = 1.0f;
    static constexpr float MIN_ZOOM_DISTANCE = 2.0f;
    static constexpr float MAX_ZOOM_DISTANCE = 100.0f;
    static constexpr float MIN_PITCH = 1.0f;
    static constexpr float MAX_PITCH = 179.0f;

    GameCamera();

    static GameCamera& getInstance() {
        static GameCamera instance;
        return instance;
    }

    void update(float frametimeS);
    void setCameraMode(CameraMode cameraType);
    void setNextCameraMode();
    std::string getCameraModeString() const;

    CameraGlu getCameraGLU() const { return cameraGlu; }
    CameraState getCameraState() const { return cameraState; }
    void setLockUserUpdate(bool lock) { lockUserUpdate = lock; }
private:
    bool lockUserUpdate = false;

    void updateInteractiveMapViewTarget();
    void updateFollowingPlayerTarget();

    static float computeInteractivePitch(float centerFracZ);
    static float computeInteractiveYaw(float centerFracX);

    void setCameraState(CameraState newCameraState) { cameraState = newCameraState; }

    float getMovingPosSensitivity() const { return movingPosSensitivity; }
    float getOrbittingDegPerPixel() const { return orbittingDegPerPixel; }

    void setMovingPosSensitivity(float newSensitivity) { movingPosSensitivity = newSensitivity; }
    void setOrbittingDegPerPixel(float newDegPerPixel) { orbittingDegPerPixel = newDegPerPixel; }

    void enableManualCamera() {
        autoCameraMoving = false;
        autoCameraOrbitting = false;
        autoCameraZooming = false;
    }
    void enableAutoCamera() {
        autoCameraMoving = true;
        autoCameraOrbitting = true;
        autoCameraZooming = true;
    }

    void setNewCameraTarget(CameraState targetState) {
        if (targetState == cameraState) { return; }
        autoCameraTarget = targetState;
        autoCameraTargetReached = false;
    }

    bool isAutoCameraAtTarget() const { return autoCameraTargetReached; }
    bool isAutoCameraOn() const { return autoCameraMoving || autoCameraOrbitting || autoCameraZooming; }

    CameraState cameraState = DEFAULT_CAMERA_STATE;
    CameraGlu cameraGlu;

    CameraMode cameraMode = CameraMode::Free;

    float movingPosSensitivity = DEFAULT_MOUSE_SENSITIVITY;
    float orbittingDegPerPixel = DEFAULT_ORBITTING_DEG_PER_PIXEL;

    bool isOrbitting = false;
    bool isMovingPos = false;

    bool autoCameraMoving = false;
    bool autoCameraZooming = false;
    bool autoCameraOrbitting = false;

    // AutoCamera target
    bool autoCameraTargetReached = false;
    CameraState autoCameraTarget;

    void updateGluFromState();
    void updateStateFromGlu();

    void handleCameraOrbitting();
    void handleCameraPosMoving();
    void handleCameraZooming();

    float frametimeNormalizedTransitionSpeed(float frametimeS) { return frametimeS * DEFAULT_CAMERA_TRANSITION_SPEED * MapFactory::TILE_SIZE; }
    void updateAutoCameraTransition(float frametimeS);

};

#endif
