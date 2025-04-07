#ifndef DIRCHANGEREQUEST_H
#define DIRCHANGEREQUEST_H

#include "MoveDir.h"
#include <chrono>

class DirChangeRequest {
private:
    MoveDir requestedMoveDir;
    uint64_t startTime;
public:
    // Threshold in which the dir change is executed
    const uint64_t DIR_CHANGE_THRESH_MS = 1000; // In ms

    DirChangeRequest(MoveDir moveDir);

    uint64_t getTimeMs();
    bool isPending();
    MoveDir getRequestedMoveDir() const;
};

#endif
