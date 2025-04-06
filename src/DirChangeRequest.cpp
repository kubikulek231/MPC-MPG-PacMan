#include "DirChangeRequest.h"

using Clock = std::chrono::steady_clock;

DirChangeRequest::DirChangeRequest(MoveDir moveDir)
    : requestedMoveDir(moveDir), startTime(Clock::now()) {
}

bool DirChangeRequest::isPending() const {
    auto now = Clock::now();
    auto elapsed = std::chrono::duration<float>(now - startTime).count();
    return elapsed < DIR_CHANGE_THRESH_SECS;
}

MoveDir DirChangeRequest::getRequestedMoveDir() const {
    return requestedMoveDir;
}

void DirChangeRequest::reset(MoveDir newDir) {
    requestedMoveDir = newDir;
    startTime = Clock::now();
}