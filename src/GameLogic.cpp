#include "GameLogic.h"
#include "Game.h"
#include <tuple>

struct EntityPosition {
    float x, y;
};

void GameLogic::update(float frameTimeSeconds) {
	Game& game = Game::getInstance();
	game.getPlayer().move(game.getMoveDir(), frameTimeSeconds, game.getMoveSpeed());
}

bool movePlayer(Game& game, MoveDir moveDir, float frameTimeSeconds, float moveSpeed) {
	Player player = game.getPlayer();
	Map map = game.getMap();
    float frameNormalizedSpeed = frameTimeSeconds * moveSpeed;
	float nextPlayerPos = getNextPlayerPos(moveDir, player.getPosition(), frameNormalizedSpeed);
	if (map.getTile(, )
}

EntityPosition GameLogic::getNextPlayerPos(MoveDir moveDir, EntityPosition currentPlayerPos, float speedNormalized) {
    EntityPosition nextPlayerPos;
    if (moveDir == MoveDir::FWD) {
        nextPlayerPos.y -= speedNormalized;
    }
    else if (moveDir == MoveDir::BWD) {
        nextPlayerPos.y += speedNormalized;
    }
    else if (moveDir == MoveDir::RIGHT) {
        nextPlayerPos.x += speedNormalized;
    }
    else {
        nextPlayerPos.x -= speedNormalized;
    }
    return nextPlayerPos;
}
