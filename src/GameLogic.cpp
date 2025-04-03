#include "GameLogic.h"
#include "Game.h"

void GameLogic::update(float frameTimeSeconds) {
	Game& game = Game::getInstance();
	game.getPlayer().move(game.getMoveDir(), frameTimeSeconds, game.getMoveSpeed());
}