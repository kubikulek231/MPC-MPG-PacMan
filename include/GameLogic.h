#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include "Entity.h"
#include "Player.h"
#include "Tile.h"
#include "DirChangeRequest.h"
#include "MoveDir.h"
#include <vector>

class Game; // Forward declaration to avoid circular dependency

class GameLogic {
public:
	static constexpr float PLAYER_SPEED = 2.5f;
	static constexpr float SNAP_DISTANCE = 0.05f;
	static void updateScore();
    static void updatePlayer();
    static void updateGhosts();
	static void updatePlayerLives();
	static void init();
};

#endif
