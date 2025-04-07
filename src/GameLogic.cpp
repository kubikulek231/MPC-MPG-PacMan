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
    Game& game = Game::getInstance();
    Player* player = game.getPlayer();

    MoveDir userMoveDir = *game.getMoveDir();

    if (userMoveDir == MoveDir::NONE) { return; }

    bool userChangedDir = game.getIsDirectionKeyPressed();
    game.setIsDirectionKeyPressed(false);

    // User pressed a dir key
    if (userChangedDir) {
        // Delete existing request
        delete dirChangeRequest;
        dirChangeRequest = nullptr;
        // If actual direction and requested direction differ, set new change request
        if (userMoveDir != actualMoveDir) {
            dirChangeRequest = new DirChangeRequest(userMoveDir);
        }
    }

    // If the same direction as before, just keep going
    if (userMoveDir == actualMoveDir) {
        delete dirChangeRequest;
        dirChangeRequest = nullptr;
        GameLogic::movePlayer(game.getFrametimeNormalizedSpeed(), actualMoveDir);
        return;
    }

    // Get player tiles for next calculations
    std::vector<Tile*> playerTiles = GameLogic::getPlayerTiles(player);

    // Try to follow a pending direction change
    if (dirChangeRequest != nullptr && dirChangeRequest->isPending()) {
        MoveDir requestedDir = dirChangeRequest->getRequestedMoveDir();
        if (GameLogic::changePlayerDirection(playerTiles, game.getFrametimeNormalizedSpeed(), requestedDir)) {
            actualMoveDir = requestedDir;
            delete dirChangeRequest;
            dirChangeRequest = nullptr;
            return;
        }
    }
    // If every attempt fails, keep moving in the same direction
    GameLogic::movePlayer(game.getFrametimeNormalizedSpeed(), actualMoveDir);
}

bool GameLogic::movePlayer(float moveSpeed, MoveDir moveDir, bool snapToTile, std::vector<Tile*> playerTiles) {
    Game& game = Game::getInstance();
    Map* map = game.getMap();
    Player* player = game.getPlayer();
    Player playerCopy = Player(*player);

    if (snapToTile && playerTiles.size() == 0) {
        std::cout << "Warning! Snap to tile is enabled but playerTiles.size() is 0!";
    }

    if (snapToTile && playerTiles.size() != 0) {
        float distance;
        Point3D closestTileOrigin = GameLogic::getClosestTileOrigin(&playerCopy, playerTiles, distance);
        if (distance < SNAP_DISTANCE) {
            playerCopy.setOrigin(closestTileOrigin);
        }
    }

    playerCopy = GameLogic::getMovedEntity(&playerCopy, moveDir, moveSpeed);
    std::vector<Tile*> movedTiles = GameLogic::getPlayerTiles(&playerCopy);
    
    if (!areAllTilesWalkable(movedTiles)) {
        return false;
    }

    game.setPlayer(playerCopy);
    return true;
}

bool GameLogic::changePlayerDirection(std::vector<Tile*> playerTiles, float moveSpeed, MoveDir moveDir) {
    Player* player = Game::getInstance().getPlayer();
    // Try without snapping at first
    if (GameLogic::movePlayer(moveSpeed, moveDir)) {
        return true;
    }
    // Try with snapping
    if (GameLogic::movePlayer(moveSpeed, moveDir, true, playerTiles)) {
        return true;
    }
    return false;
}

Player GameLogic::getMovedEntity(Player* player, MoveDir moveDir, float speedNormalized) {
    if (moveDir == MoveDir::FWD) {
        player->moveZ(-speedNormalized);
    }
    if (moveDir == MoveDir::BWD) {
        player->moveZ(speedNormalized);
    }
    if (moveDir == MoveDir::RIGHT) {
        player->moveX(speedNormalized);
    }
    if (moveDir == MoveDir::LEFT) {
        player->moveX(-speedNormalized);
    }

    return *player;
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

Point3D GameLogic::getClosestTileOrigin(Player* player, const std::vector<Tile*>& tiles, float& distance) {
    Point3D playerOrigin = player->getOrigin();

    Tile* closestTile = nullptr;
    float closestDist = FLT_MAX;

    for (Tile* tile : tiles) {
        if (!tile) continue;
        Point3D tileOrigin = tile->getOrigin();
        // Calculate the distance between player and tile
        float dist = playerOrigin.distanceTo2D(tileOrigin);
        if (dist < closestDist) {
            closestTile = tile;
            closestDist = dist;
            distance = dist;
        }
    }
    // Return the origin of the closest tile, or the player's origin if no tile is found
    return closestTile ? closestTile->getOrigin() : playerOrigin;
}
