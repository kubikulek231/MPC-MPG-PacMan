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
private:
	static DirChangeRequest* dirChangeRequest;
	static Player getMovedEntity(Player* player, MoveDir moveDir, float speedNormalized);
	static std::vector<Tile*> getPlayerTiles(Player* player);
	static bool areAllTilesWalkable(std::vector<Tile*> tiles);
	static bool moveEntityIfTileWalkable(Entity* entity, float moveSpeed, MoveDir moveDir);
	static MoveDir actualMoveDir;
public:
    static void update();
};

#endif
