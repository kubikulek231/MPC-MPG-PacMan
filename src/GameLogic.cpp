#include "GameLogic.h"
#include "Game.h"
#include <tuple>
#include <iostream>
#include "DirChangeRequest.h"
#include "MoveDir.h"

void GameLogic::init() {
	// Create ghosts path to move them into corners
	Game& game = Game::getInstance();
	auto& ghosts = game.getGhosts();
	for (size_t i = 0; i < ghosts.size(); ++i) {
		Ghost* ghost = ghosts[i];
		if (ghost->isPathEmpty()) {
			MapCorner corner = game.getMap()->corners[i % game.getMap()->corners.size()];
			Tile* targetTile = ghost->furthestTileTowardCorner(corner);
			if (targetTile) {
				ghost->createPathToTile(targetTile);
			}
		}
	}
}

void GameLogic::update() {
	Game &game = Game::getInstance();
	MoveDir& moveDir = *game.getMoveDir();
	float lastFrameTimeMs = game.getLastFrameTimeDeltaSeconds() * 1000.0f;
	game.getPlayer()->move(*game.getMoveDir(), game.getIsDirectionKeyPressed(), lastFrameTimeMs);

	// Render ghosts
	auto& ghosts = game.getGhosts();
	for (size_t i = 0; i < ghosts.size(); ++i) {
		Ghost* ghost = ghosts[i];
		if (!ghost->isPathEmpty()) {
			ghost->moveOnPath(lastFrameTimeMs);
			continue;
		}
		// Move randomly if path is empty
		ghost->moveOnRandomPath(lastFrameTimeMs);
	}
}