#include "GameLogic.h"
#include "Game.h"
#include <tuple>
#include <iostream>
#include "DirChangeRequest.h"
#include "MoveDir.h"

// Definitions of static members
DirChangeRequest* GameLogic::dirChangeRequest = nullptr;
MoveDir GameLogic::actualMoveDir = MoveDir::NONE;

void GameLogic::update() {
    // TODO:
    // Define change dir method, which will be less frametime dependent.
    // Perhaps introduce some thresholds.

    Game& game = Game::getInstance();
    MoveDir userMoveDir = *game.getMoveDir();
    Player* player = game.getPlayer();

    if (!player) return;

    // If there's no request or the user changed direction, replace the request
    if (!dirChangeRequest || dirChangeRequest->getRequestedMoveDir() != userMoveDir) {
        if (dirChangeRequest) {
            delete dirChangeRequest;
        }
        dirChangeRequest = new DirChangeRequest(userMoveDir);
    }

    MoveDir requestedDir = dirChangeRequest->getRequestedMoveDir();

    // If request is pending, move in requested direction
    if (dirChangeRequest->isPending()) {
        if (GameLogic::moveEntityIfTileWalkable(player, 2.5f, requestedDir)) {
            actualMoveDir = requestedDir;

            delete dirChangeRequest;
            dirChangeRequest = nullptr;

            return;
        }
    }

    // If the requested direction fails or request is outdated, pacman will continue in actualMoveDir
    GameLogic::moveEntityIfTileWalkable(player, 2.5f, actualMoveDir);
}

bool GameLogic::moveEntityIfTileWalkable(Entity* entity, float moveSpeed, MoveDir moveDir) {
    Game& game = Game::getInstance();
    Map* map = game.getMap();
    Player* player = game.getPlayer();
    float frameNormalizedSpeed = game.getLastFrameTimeSeconds() * moveSpeed;

    Player newPlayerEntity = GameLogic::getMovedEntity(player, moveDir, frameNormalizedSpeed);
    std::vector<Tile*> movedTiles = GameLogic::getPlayerTiles(&newPlayerEntity);
    
    if (!areAllTilesWalkable(movedTiles)) {
        return false;
    }

    game.setPlayer(newPlayerEntity);
    return true;
}

Player GameLogic::getMovedEntity(Player* player, MoveDir moveDir, float speedNormalized) {
;   // Create a copy of the player to avoid modifying the original
    Player movedPlayer = Player(*player);

    if (moveDir == MoveDir::FWD) {
        movedPlayer.moveZ(-speedNormalized);
    }
    if (moveDir == MoveDir::BWD) {
        movedPlayer.moveZ(speedNormalized);
    }
    if (moveDir == MoveDir::RIGHT) {
        movedPlayer.moveX(speedNormalized);
    }
    if (moveDir == MoveDir::LEFT) {
        movedPlayer.moveX(-speedNormalized);
    }

    return movedPlayer;
}

std::vector<Tile*> GameLogic::getPlayerTiles(Player* player) {
    Game& game = Game::getInstance();
    Map* map = game.getMap();
    BoundingBox3D* absBbox = &player->getAbsoluteBoundingBox();
    std::vector<Tile*> intersectedTiles = map->getTilesWithBoundingBox(absBbox);
    return intersectedTiles;
}

bool GameLogic::areAllTilesWalkable(std::vector<Tile*> tiles) {
    for (Tile* tile : tiles) {
        if (tile == nullptr) {
            continue;
        }
        if (!tile->isWalkable()) {
            return false;
        }
    }
    return true;
}