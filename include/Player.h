#ifndef PLAYER_H
#define PLAYER_H

#include "Entity.h"

class Player : public Entity {
public:
    Player();
    Player(const Player& other);
    Player(Point3D playerOrigin, BoundingBox3D playerBoundingBox);
    void render();
};

#endif // PLAYER_H
