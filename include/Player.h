#ifndef PLAYER_H
#define PLAYER_H

#include "Entity.h"

class Player : public Entity {
public:
    Player();
    Player(float initPosX, float initPosY);
    static constexpr float DEFAULT_SPEED = 0.01f;
    static constexpr float TILE_SIZE = 1.0f;
    void render();
private:
    EntityPosition position;
};

#endif // PLAYER_H
