#ifndef GAMELOGIC_H
#define GAMELOGIC_H

class Game; // Forward declaration to avoid circular dependency

class GameLogic {
public:
    static void update(float frameTimeSeconds);
};

#endif
