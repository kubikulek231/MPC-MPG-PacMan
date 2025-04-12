#include "MovableEntity.h"
#include <iostream>
#include <algorithm>
#include <math.h>
#include <cassert>
#include "MapFactory.h"

MovableEntity::MovableEntity(Map* map, 
                             Point3D origin, 
                             BoundingBox3D boundingBox, 
                             MoveDir moveDir, 
                             float speed, 
                             float snapDistance,                  
                             bool dirChangeRequestExpire,
                             float dirChangeRequestExpireAfterMs)
                             : Entity(origin, boundingBox) {
    this->map = map;
    this->moveDir = moveDir;
    this->speed = speed;
    this->snapDistance = snapDistance;
    this->dirChangeRequestExpire = dirChangeRequestExpire;
    this->dirChangeRequestExpireAfterMs = dirChangeRequestExpireAfterMs;
}

// TODO: If there is a wall and can not move, get the center of the current tile and center the entity on that using the move speed.


void MovableEntity::move(MoveDir requestedMoveDir, bool& isNewRequest, float frameTimeMs) {
    // If it is actually a new request
    if (isNewRequest) {
        // Delete existing request
        MovableEntity::clearDirChangeRequest();
        // If actual direction and requested direction differ, set new change request
        if (requestedMoveDir != moveDir) {
            createDirChangeRequest(requestedMoveDir, dirChangeRequestExpireAfterMs, dirChangeRequestExpire);
        }
        isNewRequest = false;
    }
    // Get player tiles for next calculations
    std::vector<Tile*> intersectingTiles = MovableEntity::intersectingTiles(this);

    // If the same direction as before, just keep going
    if (requestedMoveDir == moveDir) {
        MovableEntity::clearDirChangeRequest();
        MovableEntity::preciseMove(moveDir, frameTimeMs);
        return;
    }

    // Try to follow a pending direction change
    if (dirChangeRequest && dirChangeRequest->isPending()) {
        MoveDir requestedDir = dirChangeRequest->getRequestedMoveDir();
        if (MovableEntity::tryMove(requestedDir, frameTimeMs, true, intersectingTiles)) {
            moveDir = requestedDir;
            return;
        }
    }
    // If every attempt fails, keep moving in the same direction
    MovableEntity::preciseMove(moveDir, frameTimeMs);
}

float MovableEntity::getMoveSpeed() const {
    return speed;
}

void MovableEntity::setMoveSpeed(float speed) {
    this->speed = speed;
}

MovableEntity::MovableEntity(const MovableEntity& other) : Entity(other) {
    this->map = other.map;
    this->moveDir = other.moveDir;
    this->speed = other.speed;
    this->snapDistance = other.snapDistance;
}

void MovableEntity::createDirChangeRequest(MoveDir moveDir, float expireAfterMs, bool expire) {
    MovableEntity::clearDirChangeRequest();
    dirChangeRequest = new DirChangeRequest(moveDir, expireAfterMs, expire);
}

void MovableEntity::clearDirChangeRequest() {
    if (dirChangeRequest) {
        delete dirChangeRequest;
        dirChangeRequest = nullptr;
    }
}

MoveDir MovableEntity::getRequestedDir() {
    if (dirChangeRequest == nullptr) {
        return MoveDir::UNDEFINED;
    }
    return dirChangeRequest->getRequestedMoveDir();
}

void MovableEntity::move(float dx, float dy, float dz) {
	origin.move(dx, dy, dz);
}

void MovableEntity::move(Point3D dPoint) {
	origin.move(dPoint);
}

void MovableEntity::moveX(float dx) {
    origin.move(dx, 0, 0);
}

void MovableEntity::moveY(float dY) {
    origin.move(0, dY, 0);
}

void MovableEntity::moveZ(float dZ) {
    origin.move(0, 0, dZ);
}

void MovableEntity::moveAxis(char axis, float dAxis) {
    if (axis == 'x' || axis == 'X') {
        this->moveX(dAxis);
    }
    if (axis == 'y' || axis == 'Y') {
        this->moveY(dAxis);
    }
    if (axis == 'z' || axis == 'Z') {
        this->moveZ(dAxis);
    }
}

bool MovableEntity::tryMove(MoveDir moveDir, float frameTimeMs, bool snapToTile, const std::vector<Tile*>& intersectingTiles) {
    float moveSpeed = getFrametimeNormalizedSpeed(frameTimeMs);
    // Try to move without snapping
    MovableEntity copy = movedCopy(moveDir, moveSpeed);
    std::vector<Tile*> movedTiles = MovableEntity::intersectingTiles(&copy);

    if (areTilesWalkable(movedTiles)) {
        this->origin = copy.getOrigin();
        return true;
    }
    // If snapping is not enabled or we have no tiles to snap to
    if (!snapToTile || intersectingTiles.empty()) {
        return false;
    }

    float distance;
    Point3D closestTileOrigin = MovableEntity::closestTileOrigin(this, intersectingTiles, distance);
    closestTileOrigin.move((MapFactory::TILE_SIZE - this->boundingBox.getSizeX()) / 2, 0, (MapFactory::TILE_SIZE - this->boundingBox.getSizeY()) / 2);

    if (distance > snapDistance) {
        return false;
    }

    // Snap and try again
    copy = MovableEntity(*this);
    copy.setOrigin(closestTileOrigin);
    copy = copy.movedCopy(moveDir, moveSpeed);
    movedTiles = MovableEntity::intersectingTiles(&copy);

    if (!areTilesWalkable(movedTiles)) {
        return false;
    }

    this->origin = copy.getOrigin();
    return true;
}

MovableEntity MovableEntity::movedCopy(MoveDir moveDir, float frameTimeMs) {
    MovableEntity copy = MovableEntity(*this);
    float speed = getFrametimeNormalizedSpeed(frameTimeMs);
    if (moveDir == MoveDir::FWD) {
        copy.moveZ(-speed);
    }
    if (moveDir == MoveDir::BWD) {
        copy.moveZ(speed);
    }
    if (moveDir == MoveDir::RIGHT) {
        copy.moveX(speed);
    }
    if (moveDir == MoveDir::LEFT) {
        copy.moveX(-speed);
    }

    return copy;
}

bool MovableEntity::trySnapToTileCenter(Point3D& correctedOrigin, const std::vector<Tile*>& tiles, float maxDistance) {
    float _;
    Point3D closestOrigin = closestTileOrigin(this, tiles, _);
    Point3D currentOrigin = getOrigin();
    correctedOrigin = currentOrigin;

    float newZ = closestOrigin.z + (MapFactory::TILE_SIZE - getBoundingBox().getSizeZ()) / 2;
    if (std::abs(newZ - currentOrigin.z) <= maxDistance) {
        correctedOrigin.z = newZ;
        return true;
    }
    return false;
}

bool MovableEntity::preciseMove(MoveDir moveDir, float frameTimeMs) {
    MovableEntity copy = MovableEntity(*this);
    float speed = getFrametimeNormalizedSpeed(frameTimeMs);
    char axis;

    switch (moveDir) {
    case MoveDir::RIGHT: axis = 'x'; speed *= 1.0f; break;
    case MoveDir::LEFT:  axis = 'x'; speed *= -1.0f; break;
    case MoveDir::FWD:   axis = 'z'; speed *= -1.0f; break;
    case MoveDir::BWD:   axis = 'z'; speed *= 1.0f; break;
    default: return false; // just in case
    }
    
    // Try move full speed first
    copy.moveAxis(axis, speed);
    std::vector<Tile*> tiles = MovableEntity::intersectingTiles(&copy);
    if (MovableEntity::areTilesWalkable(tiles)) {
        this->setOrigin(copy.origin);
        return true;
    }

    // If full speed fails, try to move precisely
    copy = MovableEntity(*this);
    tiles = MovableEntity::intersectingTiles(&copy);
    float _;
    Point3D closestOrigin = closestTileOrigin(&copy, tiles, _);
    Point3D copyOrigin = copy.getOrigin();
    
    // Only move in the requested direction
    float old = (axis == 'x') ? copyOrigin.x : copyOrigin.z;
    float* copyOriginAxis = (axis == 'x') ? &copyOrigin.x : &copyOrigin.z;
    float* closestOriginAxis = (axis == 'x') ? &closestOrigin.x : &closestOrigin.z;

    float size = (axis == 'x') ? copy.getBoundingBox().getSizeX() : copy.getBoundingBox().getSizeZ();

    *copyOriginAxis = *closestOriginAxis + (MapFactory::TILE_SIZE - size) / 2.0f;

    float distance = std::abs(*copyOriginAxis - old);
    float maxMove = std::abs(speed);
    assert(distance <= maxMove && "Movement error: overshoot on precise axis move");

    copy.setOrigin(copyOrigin);
    tiles = MovableEntity::intersectingTiles(&copy);
    if (MovableEntity::areTilesWalkable(tiles)) {
        this->setOrigin(copy.origin);
        return true;
    }
    return false;
}

bool MovableEntity::areTilesWalkable(std::vector<Tile*> tiles) {
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

std::vector<Tile*> MovableEntity::intersectingTiles(const MovableEntity* movableEntity) {
    BoundingBox3D* absBbox = &movableEntity->getAbsoluteBoundingBox();
    std::vector<Tile*> intersectedTiles = movableEntity->map->getTilesWithBoundingBox(absBbox);
    return intersectedTiles;
}

Point3D MovableEntity::closestTileOrigin(const MovableEntity* movableEntity, const std::vector<Tile*>& tiles, float& distance) {
    Point3D entityOrigin = movableEntity->getOrigin();
    BoundingBox3D bbox = movableEntity->getBoundingBox();

    Tile* closestTile = nullptr;
    float closestDist = FLT_MAX;

    for (Tile* tile : tiles) {
        if (!tile) continue;

        Point3D tileOrigin = tile->getOrigin();
        float dist = entityOrigin.distanceTo2D(tileOrigin);

        if (dist < closestDist) {
            closestTile = tile;
            closestDist = dist;
            distance = dist;
        }
    }

    if (!closestTile) {
        return entityOrigin;
    }
    Point3D tileOrigin = closestTile->getOrigin();

    return Point3D(
        tileOrigin.x,
        tileOrigin.y,
        tileOrigin.z
    );
}