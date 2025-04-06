#ifndef DIRCHANGEREQUEST_H
#define DIRCHANGEREQUEST_H

#include "MoveDir.h"

class DirChangeRequest {
private:
    float dirChangeTime;
    MoveDir requestedMoveDir;
public:
    static constexpr float DIR_CHANGE_THRESH_SECS = 0.2f;

    DirChangeRequest(MoveDir moveDir);

    void updateTime(float deltaTime);
    bool isPending() const;
    MoveDir getRequestedMoveDir() const;
    void reset(MoveDir newDir);
};

#endif
