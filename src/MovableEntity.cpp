#include "MovableEntity.h"
#include <iostream>
#include <algorithm>
#include <math.h>
#include <cassert>
#include "MapFactory.h"
#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <iostream>

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

    if (requestedMoveDir == moveDir) {
        MovableEntity::clearDirChangeRequest();
        MovableEntity::preciseMove(moveDir, frameTimeMs);
        return;
    }

    // Try to follow a pending direction change
    if (dirChangeRequest && dirChangeRequest->isPending()) {
        MoveDir requestedDir = dirChangeRequest->getRequestedMoveDir();
        if (moveDir == MoveDir::NONE) { moveDir = requestedDir; }
        bool canTurn = false;
        // Change the direction right away if request is the same axis
        if (getAxisForDirection(requestedDir) == getAxisForDirection(moveDir)) {
            moveDir = requestedDir;
            MovableEntity::preciseMove(moveDir, frameTimeMs);
            return;
        }

        if (preciseMoveUntilCanTurn(requestedDir, frameTimeMs, canTurn, intersectingTiles)) {
            if (canTurn) {
                moveDir = requestedDir;
                MovableEntity::clearDirChangeRequest();
            }
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
    assert(this->boundingBox.getSizeX() < MapFactory::TILE_SIZE);
    assert(this->boundingBox.getSizeZ() < MapFactory::TILE_SIZE);
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
    Tile* closestTile = MovableEntity::closestTile(this, intersectingTiles, distance);
    Point3D centeredOrigin = tileCenteredOrigin(closestTile);

    if (distance > snapDistance) {
        return false;
    }

    // Snap and try again
    copy = MovableEntity(*this);
    copy.setOrigin(centeredOrigin);
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

// Returns false if turning IS NOT possible
// Moves entity and returns true if moving IS possible.
// If precise location for turning is hit, canTurn argument is set to true.
bool MovableEntity::preciseMoveUntilCanTurn(MoveDir requestedMoveDir, float frameTimeMs, bool& canTurn, const std::vector<Tile*>& intersectingTiles) {
    // Get current tile
    Tile* tileCurrent = currentTile(intersectingTiles);
    assert(tileCurrent && tileCurrent->isWalkable());

    // Get the next tile that will be hit
    Tile* tileToBeHit = nextTileInDirection(moveDir, tileCurrent);
    if (!tileToBeHit || !tileToBeHit->isWalkable()) {
        // Do not move
        return false;
    }

    std::cout << "tileToBeHit:" << std::endl;
    std::cout << tileToBeHit->toString() << std::endl;

    // Check if turning to the requested direction is possible after hitting the next tile
    Tile* turnedTile = nextTile(requestedMoveDir, tileToBeHit);
    if (!turnedTile || !turnedTile->isWalkable()) {
        // Do not move
        return false;
    }

    this->map->resetHighlightedTiles();
    turnedTile->setHighlight(true);
    std::cout << "turnedTile:" << std::endl;
    std::cout << turnedTile->toString() << std::endl;

    // Calculate the speed based on frame time
    float speed = getFrametimeNormalizedSpeed(frameTimeMs);
    char axis;

    // Get the axis and speed modifier for the requested direction
    if (!getAxisAndSpeedForDirection(moveDir, axis, speed)) return false;

    bool hit = false;  // Initialize hit flag
    // Try to move the entity to the closest tile in the given direction
    if (tryMoveToNextClosestTile(moveDir, this, axis, speed, hit)) {
        if (hit) canTurn = true;
        return true;
    }
    return false;
}


Tile* MovableEntity::nextTile(MoveDir moveDir, Tile* currentTile) {
    Tile* nextTile = nullptr;
    switch (moveDir) {
    case MoveDir::RIGHT: nextTile = currentTile->getTileRight(); break;
    case MoveDir::LEFT:  nextTile = currentTile->getTileLeft(); break;
    case MoveDir::FWD:   nextTile = currentTile->getTileUp(); break;
    case MoveDir::BWD:   nextTile = currentTile->getTileDown(); break;
    default:
        return currentTile;
    }
    return nextTile;
}


Tile* MovableEntity::nextTileInDirection(MoveDir moveDir, Tile* currentTile) {
    // Get the next tile in the requested direction
    Tile* tileNext = nextTile(moveDir, currentTile);
    if (!tileNext) return nullptr;

    // Determine the axis of movement based on the direction
    char axis = getAxisForDirection(moveDir);

    // Get the entity's current position and the positions of the current and next tiles
    Point3D entityDirection = this->getOrigin();
    Point3D originCurrent = tileCenteredOrigin(currentTile);
    Point3D originNext = tileCenteredOrigin(tileNext);

    // Get the values of the axis for the current and next tiles
    float currentAxisVal = originCurrent.getAxisValue(axis);
    float nextAxisVal = originNext.getAxisValue(axis);
    float entityAxisVal = entityDirection.getAxisValue(axis);

    // Check if the entity has passed the axis value of the current tile
    if ((entityAxisVal > currentAxisVal && entityAxisVal <= nextAxisVal) ||
        (entityAxisVal < currentAxisVal && entityAxisVal >= nextAxisVal)) {
        // Entity has moved past the current tile in either direction
        return tileNext;
    }

    // If the entity hasn't passed the current axis value yet, stay on the current tile
    return currentTile;
}



Tile* MovableEntity::currentTile(const std::vector<Tile*>& intersectingTiles) const {
    float _;
    return closestTile(this, intersectingTiles, _);
}

bool MovableEntity::getAxisAndSpeedForDirection(MoveDir moveDir, char& axis, float& speed) {
    switch (moveDir) {
    case MoveDir::RIGHT: axis = 'x'; speed *= 1.0f; return true;
    case MoveDir::LEFT:  axis = 'x'; speed *= -1.0f; return true;
    case MoveDir::FWD:   axis = 'z'; speed *= -1.0f; return true;
    case MoveDir::BWD:   axis = 'z'; speed *= 1.0f; return true;
    default: return false;
    }
}

char MovableEntity::getAxisForDirection(MoveDir moveDir) {
    switch (moveDir) {
    case MoveDir::RIGHT: return 'x';
    case MoveDir::LEFT:  return 'x';
    case MoveDir::FWD:   return 'z';
    case MoveDir::BWD:   return 'z';
    default: throw std::runtime_error("Fuck!");
    }
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

    if (tiles.empty()) {
        // No tiles to check, early return
        return false;
    }

    float _;
    Point3D closestOrigin = closestTile(&copy, tiles, _)->getOrigin();
    Point3D copyOrigin = copy.getOrigin();

    // Only move in the requested direction
    float old = (axis == 'x') ? copyOrigin.x : copyOrigin.z;
    float* copyOriginAxis = (axis == 'x') ? &copyOrigin.x : &copyOrigin.z;
    float* closestOriginAxis = (axis == 'x') ? &closestOrigin.x : &closestOrigin.z;

    float size = (axis == 'x') ? copy.getBoundingBox().getSizeX() : copy.getBoundingBox().getSizeZ();

    *copyOriginAxis = *closestOriginAxis + (MapFactory::TILE_SIZE - size) / 2.0f;

    float distance = std::abs(*copyOriginAxis - old);
    float maxMove = std::abs(speed);
    if (distance > maxMove) {
        // If overshoot happens, adjust the movement to avoid assertion failure
        *copyOriginAxis = old + std::copysign(maxMove, *copyOriginAxis - old);
    }

    tiles = MovableEntity::intersectingTiles(&copy);
    if (MovableEntity::areTilesWalkable(tiles)) {
        this->setOrigin(copy.origin);
        return true;
    }
    return false;
}



Point3D MovableEntity::tileCenteredOrigin(const Tile* tile) const {
    Point3D tileAlignedOrigin = Point3D(tile->getOrigin());
    tileAlignedOrigin.move((MapFactory::TILE_SIZE - this->boundingBox.getSizeX()) / 2, 0, (MapFactory::TILE_SIZE - this->boundingBox.getSizeY()) / 2);
    return tileAlignedOrigin;
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

Tile* MovableEntity::closestTile(const MovableEntity* movableEntity, const std::vector<Tile*>& tiles, float& distance) {
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
    return closestTile;
}

bool MovableEntity::tryMoveToNextClosestTile(MoveDir moveDir, MovableEntity* movableEntity, char axis, float maxMoveDistance, bool &hit) {
    MovableEntity copy = MovableEntity(*movableEntity);
    std::vector<Tile*> tiles = MovableEntity::intersectingTiles(&copy);

    float _;
    Tile* current = currentTile(tiles);
    Tile* closest = nextTileInDirection(moveDir, current);
    if (!closest) return false;

    Point3D targetOrigin = tileCenteredOrigin(closest);
    Point3D currentOrigin = copy.getOrigin();

    float currentAxisVal = (axis == 'x') ? currentOrigin.x : currentOrigin.z;
    float closestAxisVal = (axis == 'x') ? targetOrigin.x : targetOrigin.z;

    float size = (axis == 'x') ? copy.getBoundingBox().getSizeX() : copy.getBoundingBox().getSizeZ();

    float fullDistance = closestAxisVal - currentAxisVal;
    float distanceToMove = 0.0f;

    // Only move if fullDistance and maxMoveDistance have the same sign
    if ((fullDistance > 0 && maxMoveDistance > 0) || (fullDistance < 0 && maxMoveDistance < 0)) {
        // Move only up to maxMoveDistance in the correct direction
        if (std::abs(fullDistance) > std::abs(maxMoveDistance)) {
            distanceToMove = maxMoveDistance;
        }
        else {
            distanceToMove = fullDistance;
        }
    }
    // If signs differ, we move nothing (distanceToMove stays 0)
    copy.moveAxis(axis, distanceToMove);
    tiles = MovableEntity::intersectingTiles(&copy);
    if (MovableEntity::areTilesWalkable(tiles)) {
        movableEntity->setOrigin(copy.origin);
        if (std::fabs(currentAxisVal - closestAxisVal) < 0.0001f) hit = true;
        return true;
    }

    return false;
}

bool MovableEntity::tryMoveInAxis(MovableEntity* movableEntity, char axis, float distance) {
    MovableEntity copy = MovableEntity(*movableEntity);
    copy.moveAxis(axis, distance);

    std::vector<Tile*> tiles = MovableEntity::intersectingTiles(&copy);
    if (MovableEntity::areTilesWalkable(tiles)) {
        movableEntity->setOrigin(copy.origin);
        return true;
    }

    return false;
}
