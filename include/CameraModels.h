#ifndef CAMERASTRUCT_H
#define CAMERASTRUCT_H

#include <algorithm>

struct CameraState {
    float yaw = 0;
    float pitch = 0;
    float distance = 0;
    float lookAtX = 0;
    float lookAtY = 0;
    float lookAtZ = 0;

    bool operator==(const CameraState& other) const {
        const float epsilon = 0.0001f;
        return
            std::abs(yaw - other.yaw) < epsilon &&
            std::abs(pitch - other.pitch) < epsilon &&
            std::abs(distance - other.distance) < epsilon &&
            std::abs(lookAtX - other.lookAtX) < epsilon &&
            std::abs(lookAtY - other.lookAtY) < epsilon &&
            std::abs(lookAtZ - other.lookAtZ) < epsilon;
    }
};

struct CameraGlu {
    float posX = 0;
    float posY = 0;
    float posZ = 0;
    float upX = 0;
    float upY = 0;
    float upZ = 0;
    float lookAtX = 0;
    float lookAtY = 0;
    float lookAtZ = 0;

    bool operator==(const CameraGlu& other) const {
        const float epsilon = 0.0001f;
        return
            std::abs(posX - other.posX) < epsilon &&
            std::abs(posY - other.posY) < epsilon &&
            std::abs(posZ - other.posZ) < epsilon &&
            std::abs(upX - other.upX) < epsilon &&
            std::abs(upY - other.upY) < epsilon &&
            std::abs(upZ - other.upZ) < epsilon &&
            std::abs(lookAtX - other.lookAtX) < epsilon &&
            std::abs(lookAtY - other.lookAtY) < epsilon &&
            std::abs(lookAtZ - other.lookAtZ) < epsilon;
    }
};

#endif
