#include "DirChangeRequest.h"

DirChangeRequest::DirChangeRequest(MoveDir moveDir)
    : dirChangeTime(0.0f), requestedMoveDir(moveDir) {
}

void DirChangeRequest::updateTime(float deltaTime) {
    dirChangeTime += deltaTime;
}

bool DirChangeRequest::isPending() const {
    return dirChangeTime < DIR_CHANGE_THRESH_SECS;
}

MoveDir DirChangeRequest::getRequestedMoveDir() const {
    return requestedMoveDir;
}

void DirChangeRequest::reset(MoveDir newDir) {
    requestedMoveDir = newDir;
    dirChangeTime = 0.0f;
}