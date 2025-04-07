#include "DirChangeRequest.h"
#include <chrono>

using namespace std::chrono;

DirChangeRequest::DirChangeRequest(MoveDir moveDir)
    : requestedMoveDir(moveDir), startTime(DirChangeRequest::getTimeMs()) {
}

bool DirChangeRequest::isPending() {
    return (getTimeMs() - startTime) < DIR_CHANGE_THRESH_MS;
}

uint64_t DirChangeRequest::getTimeMs() {
    return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

MoveDir DirChangeRequest::getRequestedMoveDir() const {
    return requestedMoveDir;
}