#include "MovableEntity.h"
#include "MapFactory.h"
#include <iostream>
#include <algorithm>
#include <math.h>
#include <cassert>


// === Constructors ===
// Main constructor
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

// Copy constructor
MovableEntity::MovableEntity(const MovableEntity& other) : Entity(other) {
    this->map = other.map;
    this->moveDir = other.moveDir;
    this->speed = other.speed;
    this->snapDistance = other.snapDistance;
    assert(this->boundingBox.getSizeX() < MapFactory::TILE_SIZE);
    assert(this->boundingBox.getSizeZ() < MapFactory::TILE_SIZE);
}


// === Movement Interface ===
void MovableEntity::move(MoveDir requestedMoveDir, bool& isNewRequest, float frameTimeMs) {
    std::cout << this->toString() << std::endl; // Debug entity movement
    
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
        if (axisForDirection(requestedDir) == axisForDirection(moveDir)) {
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
float MovableEntity::getMoveSpeed() const { return speed; }
void MovableEntity::setMoveSpeed(float speed) { this->speed = speed; }


// === Direction Change Management ===
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

// === Frame-based Speed Calculation ===
float MovableEntity::frametimeNormalizedSpeed(float frametimeMs) const {
    float normalizedSpeed = speed * frametimeMs;
    return std::min(normalizedSpeed, MAX_NORMALIZED_SPEED);
}


// === Low-Level Movement Helpers ===
void MovableEntity::move(float dx, float dy, float dz) { origin.move(dx, dy, dz); }
void MovableEntity::move(Point3D dPoint) { origin.move(dPoint); }
void MovableEntity::moveX(float dx) { origin.move(dx, 0, 0); }
void MovableEntity::moveY(float dY) { origin.move(0, dY, 0); }
void MovableEntity::moveZ(float dZ) { origin.move(0, 0, dZ); }
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


// === Direction & Speed Helpers ===
char MovableEntity::axisForDirection(MoveDir moveDir) {
    switch (moveDir) {
    case MoveDir::RIGHT: return 'x';
    case MoveDir::LEFT:  return 'x';
    case MoveDir::FWD:   return 'z';
    case MoveDir::BWD:   return 'z';
    default: throw std::runtime_error("Fuck!");
    }
}

float MovableEntity::speedMltprForDirection(MoveDir moveDir) {
    switch (moveDir) {
    case MoveDir::RIGHT: return 1.0f;
    case MoveDir::LEFT:  return -1.0f;
    case MoveDir::FWD:   return -1.0f;
    case MoveDir::BWD:   return 1.0f;
    case MoveDir::NONE:  return 0.0f;
    default: throw std::runtime_error("Fuck!");
    }
}


// === Movement Logic ===
bool MovableEntity::preciseMove(MoveDir moveDir, float frameTimeMs) {
    MovableEntity entityCopy = MovableEntity(*this);
    float speed = frametimeNormalizedSpeed(frameTimeMs) * speedMltprForDirection(moveDir);
    char axis = axisForDirection(moveDir);

    // Try move full speed first
    entityCopy.moveAxis(axis, speed);
    std::vector<Tile*> tiles = MovableEntity::intersectingTiles(&entityCopy);

    if (MovableEntity::areTilesWalkable(tiles)) {
        this->setOrigin(entityCopy.origin);
        return true;
    }

    // If full speed fails, try to move precisely
    entityCopy = MovableEntity(*this);
    tiles = MovableEntity::intersectingTiles(&entityCopy);

    if (tiles.empty()) {
        // No tiles to check, early return
        return false;
    }

    float _;
    Point3D closestTileOrigin = tileCenteredOrigin(closestTile(&entityCopy, tiles, _));
    Point3D entityCopyOrigin = entityCopy.getOrigin();

    // Only move in the requested direction
    float tileAxisValue = closestTileOrigin.getAxisValue(axis);
    float entityCopyAxisValue = entityCopyOrigin.getAxisValue(axis);

    float distance = std::abs(tileAxisValue - entityCopyAxisValue);
    assert(distance <= std::abs(speed));

    // Align both tile and entity axes
    entityCopyOrigin.setAxisValue(axis, tileAxisValue);
    
    // Check just to be sure
    tiles = MovableEntity::intersectingTiles(&entityCopy);
    if (MovableEntity::areTilesWalkable(tiles)) {
        this->setOrigin(entityCopyOrigin);
        return true;
    }
    return false;
}

// Attempts to move the entity in its current direction until it reaches a point
// where a turn to the requested direction is possible.
// Returns true if movement occurs; false otherwise.
// Sets 'canTurn' to true if the entity reaches a precise location where turning is possible.
bool MovableEntity::preciseMoveUntilCanTurn(MoveDir requestedMoveDir, float frameTimeMs, bool& canTurn, const std::vector<Tile*>& intersectingTiles) {
    // Determine the current tile the entity is on
    Tile* tileCurrent = currentTile(intersectingTiles);
    assert(tileCurrent && tileCurrent->isWalkable());

    // Identify the tile in the current movement direction
    Tile* tileToBeHit = nextTileInDirection(moveDir, tileCurrent);
    if (!tileToBeHit || !tileToBeHit->isWalkable()) {
        // Abort movement if next tile is invalid or not walkable
        return false;
    }

    // Check if turning to the requested direction is viable after moving
    Tile* turnedTile = nextTile(requestedMoveDir, tileToBeHit);
    if (!turnedTile || !turnedTile->isWalkable()) {
        // Abort movement if the turn is not possible
        return false;
    }

    // Get movement axis and adjust speed for current direction
    float speed = frametimeNormalizedSpeed(frameTimeMs) * speedMltprForDirection(moveDir);
    char axis = axisForDirection(moveDir);

    bool hit = false;
    // Move toward the next closest tile; set canTurn if destination reached
    if (tryMoveToNextClosestTile(moveDir, this, axis, speed, hit)) {
        if (hit) canTurn = true;
        return true;
    }

    return false;
}

bool MovableEntity::tryMoveToNextClosestTile(MoveDir moveDir, MovableEntity* movableEntity, char axis, float maxMoveDistance, bool& hit) {
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


// === Tile Navigation & Positioning ===
Tile* MovableEntity::currentTile(const std::vector<Tile*>& intersectingTiles) const {
    float _;
    return closestTile(this, intersectingTiles, _);
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
    char axis = axisForDirection(moveDir);

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

Point3D MovableEntity::tileCenteredOrigin(const Tile* tile) const {
    Point3D tileAlignedOrigin = Point3D(tile->getOrigin());
    tileAlignedOrigin.move((MapFactory::TILE_SIZE - this->boundingBox.getSizeX()) / 2, 0, (MapFactory::TILE_SIZE - this->boundingBox.getSizeY()) / 2);
    return tileAlignedOrigin;
}

// === Tile Utility Functions ===
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