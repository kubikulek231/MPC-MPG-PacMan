#include "GameLogic.h"
#include "Game.h"
#include <tuple>
#include <iostream>
#include "DirChangeRequest.h"
#include "MoveDir.h"
#include "Macro.h"

void GameLogic::init() {
	// Create ghosts path to move them into corners
	Game& game = Game::getInstance();
	std::vector<Ghost*>& ghosts = game.getGhosts();
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

void GameLogic::updateScore() {
	Game& game = Game::getInstance();
	Map& map = *game.getMap();
	std::cout << "Total collected pellets: " << game.gameCollectedPellets << std::endl;
	if (map.areAllPelletsCollected()) {
		std::cout << "GAME WON!" << std::endl;
		std::cout << "Collected pellets: " << game.gameCollectedPellets << std::endl;
		while (true);
	}
}

void GameLogic::updatePlayer() {
	Game &game = Game::getInstance();
	MoveDir& moveDir = *game.getMoveDir();
	Player& player = *game.getPlayer();
	float lastFrameTimeMs = game.getLastFrameTimeDeltaSeconds() * 1000.0f;
	player.move(*game.getMoveDir(), game.getIsDirectionKeyPressed(), lastFrameTimeMs);
	player.update(game.gameCollectedPellets);
}

void GameLogic::updateGhosts() {
	Game& game = Game::getInstance();
	MoveDir& moveDir = *game.getMoveDir();
	float lastFrameTimeMs = game.getLastFrameTimeDeltaSeconds() * 1000.0f;

	// Do not update ghost movement until player chooses moveDir
	if (moveDir == MoveDir::UNDEFINED) { return; }

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

void GameLogic::updatePlayerLives() {
	Game& game = Game::getInstance();
	std::vector<Ghost*>& ghosts = game.getGhosts();
	Player& player = *game.getPlayer();

	if (player.getIsInvincible()) { return; }
	for (size_t i = 0; i < ghosts.size(); ++i) {
		Ghost* ghost = ghosts[i];
		if (ghost->intersects(player)) {
			game.setPlayerLives(game.getPlayerLives() - 1);
			player.setIsInvincible(3000);
		}
	}
}