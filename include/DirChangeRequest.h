#ifndef DIRCHANGEREQUEST_H
#define DIRCHANGEREQUEST_H

#include "MoveDir.h"
#include <chrono>

class DirChangeRequest {
private:
    MoveDir requestedMoveDir;
    std::chrono::steady_clock::time_point startTime;
public:
    // Threshold in which the dir change is executed
    static constexpr float DIR_CHANGE_THRESH_SECS = 1.0f;

    DirChangeRequest(MoveDir moveDir);

    bool isPending() const;
    MoveDir getRequestedMoveDir() const;
    void reset(MoveDir newDir);
};

#endif
