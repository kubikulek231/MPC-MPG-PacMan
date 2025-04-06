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
    // SOLUTION:
    // Define a method which will move the entity to the nearest center of tile.
    // After that the direction will be changed.
    Game& game = Game::getInstance();
    Player* player = game.getPlayer();

    MoveDir userMoveDir = *game.getMoveDir();

    if (userMoveDir == MoveDir::NONE) { return; }

    bool userChangedDir = game.getIsDirectionChanged();
    game.setIsDirectionChanged(false);

    // User pressed a dir key
    if (userChangedDir) {
        // Delete existing request
        delete dirChangeRequest;
        dirChangeRequest = nullptr;
        // Try to move in requested direction
        if (GameLogic::moveEntityIfTileWalkable(player, game.getFrametimeNormalizedSpeed(), userMoveDir)) {
            // On success, set the actual dir and return
            actualMoveDir = userMoveDir;
            return;
        }
        // Set a new request on fail
        dirChangeRequest = new DirChangeRequest(userMoveDir);
    }

    // If the same direction as before, just keep going
    if (userMoveDir == actualMoveDir) {
        delete dirChangeRequest;
        dirChangeRequest = nullptr;
        GameLogic::moveEntityIfTileWalkable(player, game.getFrametimeNormalizedSpeed(), actualMoveDir);
        return;
    }

    // Get player tiles for next calculations
    std::vector<Tile*> playerTiles = GameLogic::getPlayerTiles(player);

    // Try to follow a pending direction change
    if (dirChangeRequest != nullptr && dirChangeRequest->isPending()) {
        MoveDir requestedDir = dirChangeRequest->getRequestedMoveDir();
        if (GameLogic::moveEntityIfTileWalkable(player, game.getFrametimeNormalizedSpeed(), requestedDir)) {
            actualMoveDir = requestedDir;
            delete dirChangeRequest;
            dirChangeRequest = nullptr;
            return;
        }
        // If first attempt fails, snap when within threshold to tile center
        float distance;
        Point3D playerClosestTileCenter = GameLogic::getClosestTileCenter(player, playerTiles, distance);
        if (distance < SNAP_DISTANCE) {
            player->setOrigin(playerClosestTileCenter);
            if (!GameLogic::moveEntityIfTileWalkable(player, game.getFrametimeNormalizedSpeed(), requestedDir)) {
                // This should not happen
                throw std::runtime_error("Tile move failed unexpectedly.");
            }
            actualMoveDir = requestedDir;
            delete dirChangeRequest;
            dirChangeRequest = nullptr;
            return;
        }
    }
    // If every attempt fails, move in the current direction
    GameLogic::moveEntityIfTileWalkable(player, game.getFrametimeNormalizedSpeed(), actualMoveDir);
}


bool GameLogic::moveEntityIfTileWalkable(Entity* entity, float moveSpeed, MoveDir moveDir) {
    Game& game = Game::getInstance();
    Map* map = game.getMap();
    Player* player = game.getPlayer();

    Player newPlayerEntity = GameLogic::getMovedEntity(player, moveDir, moveSpeed);
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

Point3D GameLogic::getNextTileCenter(Player* player, const std::vector<Tile*>& tiles, MoveDir dir, float& distance) {
    Point3D playerCenter = player->getAbsoluteCenterPoint();

    Tile* closestTile = nullptr;
    float closestDist = FLT_MAX;

    // They both lie on XZ plane
    playerCenter.y = MapFactory::MAP_Y;

    for (Tile* tile : tiles) {
        if (!tile) continue;
        Point3D tileCenter = tile->getCenterPoint();
        // Filter by direction
        bool inDirection = false;
        switch (dir) {
        case MoveDir::FWD:    inDirection = tileCenter.z < playerCenter.z; break;
        case MoveDir::BWD:    inDirection = tileCenter.z > playerCenter.z; break;
        case MoveDir::LEFT:   inDirection = tileCenter.x < playerCenter.x; break;
        case MoveDir::RIGHT:  inDirection = tileCenter.x > playerCenter.x; break;
        default: break;
        }
        if (!inDirection) continue;
        // Calculate distance to the tile
        float dist = playerCenter.distanceTo(tileCenter);
        if (dist < closestDist) {
            closestTile = tile;
            closestDist = dist;
            distance = dist;
        }
    }
    // Return the center of the closest tile or the player's center if no tile found
    return closestTile ? closestTile->getCenterPoint() : playerCenter;
}

Point3D GameLogic::getClosestTileCenter(Player* player, const std::vector<Tile*>& tiles, float& distance) {
    Point3D playerCenter = player->getAbsoluteCenterPoint();

    Tile* closestTile = nullptr;
    float closestDist = FLT_MAX;

    // They both lie on XZ plane
    playerCenter.y = MapFactory::MAP_Y;

    for (Tile* tile : tiles) {
        if (!tile) continue;
        Point3D tileCenter = tile->getCenterPoint();
        // Calculate the distance between player and tile
        float dist = playerCenter.distanceTo(tileCenter);
        if (dist < closestDist) {
            closestTile = tile;
            closestDist = dist;
            distance = dist;
        }
    }
    // Return the center of the closest tile, or the player's center if no tile is found
    return closestTile ? closestTile->getCenterPoint() : playerCenter;
}
