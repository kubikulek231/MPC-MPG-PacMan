#include "GameLogic.h"
#include "Game.h"
#include <tuple>
#include <iostream>
#include "DirChangeRequest.h"
#include "MoveDir.h"

void GameLogic::update() {
	Game &game = Game::getInstance();
	MoveDir& moveDir = *game.getMoveDir();
	if (moveDir == MoveDir::NONE) { return; }
	float lastFrameTimeMs = game.getLastFrameTimeDeltaSeconds() * 1000.0f;
	game.getPlayer()->move(*game.getMoveDir(), game.getIsDirectionKeyPressed(), lastFrameTimeMs);
	// Render ghosts
	//for (Ghost* ghost : game.getGhosts()) {
	//	ghost->autoMove(lastFrameTimeMs);
	//	ghost->moveToTile(lastFrameTimeMs, game.getMap()->getTileWithRowCol(18, 15));
	//}
	//
	//game.getGhosts()[0]->autoMove(lastFrameTimeMs);
	if (game.getGhosts()[0]->isPathEmpty()) {
		Tile* targetTile = game.getMap()->getTileAt(4, 1);
		game.getGhosts()[0]->createPathToTile(targetTile);
	}
	game.getGhosts()[0]->moveOnPath(lastFrameTimeMs);
}