#include "GameLogic.h"
#include "Game.h"
#include <tuple>
#include <iostream>
#include "DirChangeRequest.h"
#include "MoveDir.h"
#include "Macro.h"
#include "GameCamera.h"
#include "GameControl.h"
#include "GameSounds.h"

void GameLogic::initLevel() {
	// Create ghosts path to move them into corners
	Game& game = Game::getInstance();
	std::vector<Ghost*>& ghosts = game.getGhosts();
	for (size_t i = 0; i < ghosts.size(); ++i) {
		Ghost* ghost = ghosts[i];
		ghost->clearMovePath();
		MapCorner corner = game.getMap()->corners[i % game.getMap()->corners.size()];
		Tile* targetTile = ghost->furthestTileTowardCorner(corner);
		if (targetTile) {
			ghost->createAndSetPathToTileWhenPossible(targetTile);
		}
	}
	game.getPlayer()->forceSetMoveDir(MoveDir::NONE);
}

void GameLogic::updateScore() {
	Game& game = Game::getInstance();
	Map& map = *game.getMap();
	game.setTotalScore(game.gameCollectedPellets);
	if (map.areAllPelletsCollected()) {
		// Set new level
		game.setCurrentLevel(game.getCurrentLevel() + 1);
		game.initNewLevel();
	}
}

void GameLogic::updatePlayer() {
	Game &game = Game::getInstance();
	GameControl& gc = GameControl::getInstance();
	MoveDir moveDir = gc.getMoveDir();
	Player& player = *game.getPlayer();
	float lastframetimeS = game.getLastFrameTimeDeltaSeconds();

	// Move and update only if animation is finished
	if (game.isPlayerDying()) {
		if (player.updateDeathAnimation(lastframetimeS)) { 
			game.resetPlayerDying(); 
			game.resetLevelOnDeath();
		};
		return;
	}

	player.move(moveDir, gc.getMovementChanged(), lastframetimeS);
	player.update(game.gameCollectedPellets);
}

void GameLogic::updateGhosts() {
	Game& game = Game::getInstance();
	MoveDir moveDir = GameControl::getInstance().getMoveDir();
	float lastFrametimeS = game.getLastFrameTimeDeltaSeconds();

	// Do not update ghost movement until player chooses moveDir
	if (moveDir == MoveDir::UNDEFINED || moveDir == MoveDir::NONE) { return; }

	GameUserInput& gc = GameUserInput::getInstance();

	// Move ghosts only when player is not dying
	if (game.isPlayerDying()) { return; }

	auto& ghosts = game.getGhosts();
	for (size_t i = 0; i < ghosts.size(); ++i) {
		Ghost* ghost = ghosts[i];
		if (!ghost->isPathEmpty()) {
			ghost->moveOnPath(lastFrametimeS);
			continue;
		}
		// Move randomly if path is empty
		ghost->moveOnRandomPath(lastFrametimeS);
	}

	//// Debug
	//if (gc.isKeyFlagPressed('x')) {
	//	gc.resetKeyFlagPressed('x');
	//	for (size_t i = 0; i < ghosts.size(); ++i) {
	//		Map* map = game.getMap();
	//		Ghost* ghost = ghosts[i];
	//		Tile* tile = map->getTileAt(17, 1);
	//		if (!ghost->isPathEmpty()) {
	//			ghost->createAndSetPathToTileWhenPossible(tile);
	//			continue;
	//		}
	//		// Move randomly if path is empty
	//		ghost->moveOnRandomPath(lastFrametimeS);
	//	}
	//}

	//if (gc.isKeyFlagPressed('y')) {
	//	gc.resetKeyFlagPressed('y');
	//	for (size_t i = 0; i < ghosts.size(); ++i) {
	//		Map* map = game.getMap();
	//		Ghost* ghost = ghosts[i];
	//		Tile* tile = map->getTileAt(17, 26);
	//		if (!ghost->isPathEmpty()) {
	//			ghost->createAndSetPathToTileWhenPossible(tile);
	//			continue;
	//		}
	//		// Move randomly if path is empty
	//		ghost->moveOnRandomPath(lastFrametimeS);
	//	}
	//}
}

void GameLogic::updatePlayerLives() {
	Game& game = Game::getInstance();
	std::vector<Ghost*>& ghosts = game.getGhosts();
	Player& player = *game.getPlayer();

	if (game.isPlayerDying()) { return; }
	for (size_t i = 0; i < ghosts.size(); ++i) {
		Ghost* ghost = ghosts[i];
		if (ghost->intersects(player)) {
			if (player.getIsInvincible()) { continue; }
			game.killPlayer();
		}
	}
}