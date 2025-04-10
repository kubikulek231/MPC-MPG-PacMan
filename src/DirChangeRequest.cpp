#include "DirChangeRequest.h"
#include <chrono>

using namespace std::chrono;

DirChangeRequest::DirChangeRequest(MoveDir moveDir, float expireAfterMs, bool expire) {
    this->startTimeMs = DirChangeRequest::getTimeMs();
    this->requestedMoveDir = moveDir;
    this->expireAfterMs = expireAfterMs;
    this->expire = expire;
}

bool DirChangeRequest::isPending() {
    if (!expire) return true;
    return (getTimeMs() - startTimeMs) < expireAfterMs;
}

uint64_t DirChangeRequest::getTimeMs() {
    return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

MoveDir DirChangeRequest::getRequestedMoveDir() {
    if (!DirChangeRequest::isPending()) { return MoveDir::UNDEFINED; }
    return requestedMoveDir;
}