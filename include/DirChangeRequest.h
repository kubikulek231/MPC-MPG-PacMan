#ifndef DIRCHANGEREQUEST_H
#define DIRCHANGEREQUEST_H

#include "MoveDir.h"
#include <chrono>

class DirChangeRequest {
private:
    bool expire = false;
    float expireAfterMs;
    MoveDir requestedMoveDir;
    uint64_t startTimeMs;
public:
    DirChangeRequest(MoveDir moveDir, float expireAfterMs, bool expire = true);

    bool isPending();
    uint64_t getTimeMs();

    MoveDir getRequestedMoveDir();
};

#endif
