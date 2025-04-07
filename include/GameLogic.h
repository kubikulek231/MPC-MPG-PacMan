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
	static MoveDir actualMoveDir;
	static DirChangeRequest* dirChangeRequest;
	static Player getMovedEntity(Player* player, MoveDir moveDir, float speedNormalized);
	static std::vector<Tile*> getPlayerTiles(Player* player);
	static bool areAllTilesWalkable(std::vector<Tile*> tiles);
	static bool movePlayer(float moveSpeed, MoveDir moveDir, bool snapToTile = false, std::vector<Tile*> playerTiles = {});
	static bool changePlayerDirection(std::vector<Tile*> playerTiles, float moveSpeed, MoveDir moveDir);
	static Point3D getClosestTileOrigin(Player* player, const std::vector<Tile*>& tiles, float& distance);
public:
	static constexpr float PLAYER_SPEED = 2.5f;
	static constexpr float SNAP_DISTANCE = 0.05f;
    static void update();
};

#endif
