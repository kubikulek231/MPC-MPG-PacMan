#ifndef PLAYER_H
#define PLAYER_H

#include "Player.h"

enum MoveDir
{
    FWD = 0,
    BWD = 1,
    LEFT = 2,
    RIGHT = 3,
};

class Player {
public:
    Player(float initialPosX, float initialPosY);
    static constexpr float TILE_SIZE = 1.0f;
    void render();
    void move(MoveDir moveDir);

private:
    float posX;
    float posY;
    static constexpr float SPEED = 1.0f;
};

#endif // MAP_FACTORY_H
