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
	game.getPlayer()->move(*game.getMoveDir(), game.getIsDirectionKeyPressed(), game.getFrametimeNormalizedSpeed());
}