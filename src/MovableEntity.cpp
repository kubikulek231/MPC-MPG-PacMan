#include "MovableEntity.h"
#include "MapFactory.h"
#include <iostream>
#include <algorithm>
#include <math.h>
#include "Macro.h"


// === Constructors ===
// Main constructor
MovableEntity::MovableEntity(Map* map, 
                             Point3D origin, 
                             BoundingBox3D boundingBox, 
                             MoveDir moveDir, 
                             float speed, 
                             float snapDistance,                  
                             bool dirChangeRequestExpire,
                             uint64_t dirChangeRequestExpireAfterMs)
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
    ASSERT_MSG(this->boundingBox.getSizeX() < MapFactory::TILE_SIZE, "Bounding box size in X dimension of MovableEntity must be less than MapFactory::TILE_SIZE!");
    ASSERT_MSG(this->boundingBox.getSizeZ() < MapFactory::TILE_SIZE, "Bounding box size in Z dimension of MovableEntity must be less than MapFactory::TILE_SIZE!");
}


// === Movement Interface ===
bool MovableEntity::move(MoveDir requestedMoveDir, bool& isNewRequest, float frametimeS) {
    if (moveDir == MoveDir::NONE || moveDir == MoveDir::UNDEFINED) {
        // Set initial moveDir
        if (requestedMoveDir != MoveDir::NONE && requestedMoveDir != MoveDir::UNDEFINED) {
            moveDir = requestedMoveDir;
        }
        // No requstedMoveDir nor moveDir
        return false;
    }
    bool moved;
    // std::cout << this->toString() << std::endl; // Debug entity movement
    Tile* tile = currentTile(intersectingTiles(this));
    //if (tile) { tile->setHighlight(true); }
    // If it is actually a new request
    if (isNewRequest) {
        // Delete existing request
        MovableEntity::clearDirChangeRequest();
        // If actual direction and requested direction differ, set new change request
        if (requestedMoveDir != moveDir) {
            createDirChangeRequest(requestedMoveDir, this->dirChangeRequestExpireAfterMs, this->dirChangeRequestExpire);
        }
        isNewRequest = false;
    }
    // Get player tiles for next calculations
    std::vector<Tile*> intersectingTiles = MovableEntity::intersectingTiles(this);

    // Do not move when out of bounds
    if (intersectingTiles.empty()) { return false; }

    if (requestedMoveDir == moveDir) {
        MovableEntity::clearDirChangeRequest();
        MovableEntity::preciseMove(moveDir, frametimeS, moved);
        return moved;
    }

    // Try to follow a pending direction change
    if (dirChangeRequest && dirChangeRequest->isPending()) {
        MoveDir requestedDir = dirChangeRequest->getRequestedMoveDir();
        if (moveDir == MoveDir::NONE) { moveDir = requestedDir; }
        bool canTurn = false;
        // Change the direction right away if request is the same axis
        if (axisForDirection(requestedDir) == axisForDirection(moveDir)) {
            moveDir = requestedDir;
            MovableEntity::preciseMove(moveDir, frametimeS, moved);
            return moved;
        }
        if (preciseMoveUntilCanTurn(requestedDir, frametimeS, canTurn, moved, intersectingTiles)) {
            if (canTurn) {
                moveDir = requestedDir;
                MovableEntity::clearDirChangeRequest();
            }
            return moved;
        }
    }
    // If every attempt fails, keep moving in the same direction
    MovableEntity::preciseMove(moveDir, frametimeS, moved);
    return moved;
}

float MovableEntity::getMoveSpeed() const { return speed; }
void MovableEntity::setMoveSpeed(float speed) { this->speed = speed; }

float MovableEntity::getMoveDirRotationAngle() const {
    if (moveDir == MoveDir::FWD) { return 0.0f; }
    if (moveDir == MoveDir::BWD) { return 180.0f; }
    if (moveDir == MoveDir::LEFT) { return 90.0f; }
    if (moveDir == MoveDir::RIGHT) { return 270.0f; }
    return 180.0f; // Default
}

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

MoveDir MovableEntity::getOppositeMoveDir(MoveDir moveDir) {
    if (moveDir == MoveDir::LEFT) { return MoveDir::RIGHT; }
    if (moveDir == MoveDir::RIGHT) { return MoveDir::LEFT; }
    if (moveDir == MoveDir::FWD) { return MoveDir::BWD; }
    if (moveDir == MoveDir::BWD) { return MoveDir::FWD; }
    return moveDir;
}

// === Frame-based Speed Calculation ===
float MovableEntity::frametimeNormalizedSpeed(float frametimeS) const {
    float normalizedSpeed = speed * frametimeS;
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
bool MovableEntity::preciseMove(MoveDir moveDir, float frametimeS, bool &moved) {
    moved = false;

    auto initialTiles = intersectingTiles(this);
    auto initialTile = currentTile(initialTiles);

    MovableEntity entityCopy = MovableEntity(*this);
    float fullSpeed = frametimeNormalizedSpeed(frametimeS) * speedMltprForDirection(moveDir);
    char moveAxis = axisForDirection(moveDir);

    bool headingOut = headingOutOfMap(moveDir, initialTile);

    // Move at full speed if not heading out of map bounds
    if (!headingOut) {
        entityCopy.moveAxis(moveAxis, fullSpeed);
        std::vector<Tile*> tiles = MovableEntity::intersectingTiles(&entityCopy);

        if (MovableEntity::areTilesWalkable(tiles)) {
            this->setOrigin(entityCopy.origin);
            moved = true;
            return true;
        }
    }
    
    // If full speed fails, try to move precisely
    entityCopy = MovableEntity(*this);

    float _;
    // Determine if heading out of bounds
    Point3D targetTileOrigin;
    if (headingOut) {
        targetTileOrigin = furthestPossibleEntityOriginPoint(moveDir, initialTile);
    }
    else {
        targetTileOrigin = entityCenteredOrigin(closestTile(&entityCopy, initialTiles, _));
    }

    Point3D entityCopyOrigin = entityCopy.getOrigin();

    // Only move in the requested direction
    float targetAxisValue = targetTileOrigin.getAxisValue(moveAxis);
    float entityCopyAxisValue = entityCopyOrigin.getAxisValue(moveAxis);

    // Get distance between the target point and actual point
    float distanceToMove = targetAxisValue - entityCopyAxisValue;

    // Clamp the distanceToMove to fullSpeed (move at fullSpeed max)
    distanceToMove = clampMoveDistance(distanceToMove, frametimeNormalizedSpeed(frametimeS));
    
    // Move the copy
    entityCopy.moveAxis(moveAxis, distanceToMove);

    // Check just to be sure
    auto finalTiles = MovableEntity::intersectingTiles(&entityCopy);
    if (MovableEntity::areTilesWalkable(finalTiles)) {
        this->setOrigin(entityCopy.getOrigin());
        if (distanceToMove != 0.f) moved = true;
        return true;
    }
    return false;
}

// Attempts to move the entity in its current direction until it reaches a point
// where a turn to the requested direction is possible.
// Returns true if movement occurs; false otherwise.
// Sets 'canTurn' to true if the entity reaches a precise location where turning is possible.
bool MovableEntity::preciseMoveUntilCanTurn(MoveDir requestedMoveDir, float frametimeS, bool& canTurn, bool& moved, const std::vector<Tile*>& intersectingTiles) {
    moved = false;
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
    float speed = frametimeNormalizedSpeed(frametimeS) * speedMltprForDirection(moveDir);
    char axis = axisForDirection(moveDir);

    bool inCenter = false;
    // Move toward the next closest tile; set canTurn if destination reached
    if (tryMoveToNextClosestTile(moveDir, this, axis, speed, inCenter, moved)) {
        if (inCenter) {
            canTurn = true; 
        }
        return true;
    }

    return false;
}

bool MovableEntity::tryMoveToNextClosestTile(MoveDir moveDir, MovableEntity* movableEntity, char axis, float maxMoveDistance, bool& hit, bool& moved, bool proceedToNextTile) {
    moved = false;
    MovableEntity copy = MovableEntity(*movableEntity);
    std::vector<Tile*> tiles = MovableEntity::intersectingTiles(&copy);

    float _;
    Tile* current = currentTile(tiles);
    Tile* closest = nextTileInDirection(moveDir, current);
    if (!closest) return false;

    Point3D targetOrigin = entityCenteredOrigin(closest);
    Point3D currentOrigin = copy.getOrigin();

    float currentAxisVal = (axis == 'x') ? currentOrigin.x : currentOrigin.z;
    float closestAxisVal = (axis == 'x') ? targetOrigin.x : targetOrigin.z;

    float size = (axis == 'x') ? copy.getBoundingBox().getSizeX() : copy.getBoundingBox().getSizeZ();

    float fullDistance = closestAxisVal - currentAxisVal;

    // Already at the center of the tile, proceed to next in the direction if desired
    if (fullDistance == 0 && proceedToNextTile) {
        copy = MovableEntity(*movableEntity);
        targetOrigin = entityCenteredOrigin(nextTile(moveDir, current));
        currentOrigin = copy.getOrigin();

        currentAxisVal = (axis == 'x') ? currentOrigin.x : currentOrigin.z;
        closestAxisVal = (axis == 'x') ? targetOrigin.x : targetOrigin.z;

        size = (axis == 'x') ? copy.getBoundingBox().getSizeX() : copy.getBoundingBox().getSizeZ();

        fullDistance = closestAxisVal - currentAxisVal;
    }

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

        // Update current position from the modified copy
        float newAxisVal = (axis == 'x') ? copy.getOrigin().x : copy.getOrigin().z;
        if (std::fabs(newAxisVal - closestAxisVal) < 0.0001f) hit = true;

        if (distanceToMove != 0.f) moved = true;
        return true;
    }

    return false;
}

void MovableEntity::teleport(MoveDir moveDir) {
    auto tiles = this->intersectingTiles(this);
    Tile* tile = this->currentTile(tiles);
    if (!tile) return;

    // Only teleport if the next tile is a teleport
    if (!tile || tile->getTileType() != TileType::TELEPORT) return;

    Point3D teleportPoint = furthestPossibleEntityOriginPoint(moveDir, tile);

    char movingAxis = axisForDirection(moveDir);

    if (teleportPoint.getAxisValue(movingAxis) != this->getOrigin().getAxisValue(movingAxis)) { return; }

    Tile* targetTile = nullptr;

    float originOffsetX = 0;
    float originOffsetZ = 0;

    switch (moveDir) {
    case MoveDir::FWD:
        targetTile = tile->getTileUp();
        break;
    case MoveDir::BWD:
        targetTile = tile->getTileDown();
        break;
    case MoveDir::LEFT:
        targetTile = tile->getTileLeft();
        break;
    case MoveDir::RIGHT:
        targetTile = tile->getTileRight();
        break;
    default:
        return;
    }

    ASSERT_MSG(
        targetTile && targetTile->getTileType() == TileType::TELEPORT,
        "Teleport exit must be a teleport tile and must exist!"
    );
    MoveDir oppositeMoveDir = getOppositeMoveDir(moveDir);
    Point3D oppositeTeleportPoint = furthestPossibleEntityOriginPoint(oppositeMoveDir, targetTile);
    float teleportedAxisValue = oppositeTeleportPoint.getAxisValue(movingAxis);
    Point3D teleportedEntityOrigin = Point3D(oppositeTeleportPoint);
    teleportedEntityOrigin.setAxisValue(movingAxis, teleportedAxisValue);
    this->setOrigin(teleportedEntityOrigin);
}

Point3D MovableEntity::furthestPossibleEntityOriginPoint(MoveDir moveDir, Tile* currentTile) {
    ASSERT_MSG(currentTile, "Tile must not be nullptr!");

    Point3D origin = Point3D(0, 0, 0);

    int row = currentTile->getTileRow();
    int col = currentTile->getTileCol();

    if (row == MapFactory::MAP_HEIGHT - 1 && moveDir == MoveDir::FWD) {
        origin = entityCenteredOrigin(currentTile);
        origin.move(0, 0, MapFactory::TILE_SIZE / 2.0f);
        return origin;
    }

    if (col == MapFactory::MAP_WIDTH - 1 && moveDir == MoveDir::RIGHT) {
        origin = entityCenteredOrigin(currentTile);
        origin.move(MapFactory::TILE_SIZE / 2.0f, 0, 0);
        return origin;
    }

    if (row == 0 && moveDir == MoveDir::BWD) {
        origin = entityCenteredOrigin(currentTile);
        origin.move(0, 0, -MapFactory::TILE_SIZE / 2.0f);
        return origin;
    }

    if (col == 0 && moveDir == MoveDir::LEFT) {
        origin = entityCenteredOrigin(currentTile);
        origin.move(-MapFactory::TILE_SIZE / 2.0f, 0, 0);
        return origin;
    }

    return origin;  // Tile exists and is within bounds
}

bool MovableEntity::headingOutOfMap(MoveDir moveDir, Tile* currentTile) {
    ASSERT_MSG(currentTile, "Tile must not be nullptr!");

    int row = currentTile->getTileRow();
    int col = currentTile->getTileCol();

    if (row == MapFactory::MAP_HEIGHT - 1 && moveDir == MoveDir::FWD) {
        return true;
    }

    if (col == MapFactory::MAP_WIDTH - 1 && moveDir == MoveDir::RIGHT) {
        return true;
    }

    if (row == 0 && moveDir == MoveDir::BWD) {
        return true;
    }

    if (col == 0 && moveDir == MoveDir::LEFT) {
        return true;
    }
    return false;
}

// === Tile Navigation & Positioning ===
Tile* MovableEntity::currentTile(const std::vector<Tile*>& intersectingTiles) const {
    float _;
    Tile* closest = closestTile(this, intersectingTiles, _);
    return closest;
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

MoveDir MovableEntity::dirToTile(Tile* fromTile, Tile* toTile) {
    if (!fromTile || !toTile) return MoveDir::NONE;
    if (fromTile->getTileUp() == toTile) return MoveDir::FWD;
    if (fromTile->getTileDown() == toTile) return MoveDir::BWD;
    if (fromTile->getTileLeft() == toTile) return MoveDir::LEFT;
    if (fromTile->getTileRight() == toTile) return MoveDir::RIGHT;
    return MoveDir::NONE; // Not adjacent
}

Tile* MovableEntity::nextTileInDirection(MoveDir moveDir, Tile* currentTile) {
    // Get the next tile in the requested direction
    Tile* tileNext = nextTile(moveDir, currentTile);
    if (!tileNext) return nullptr;

    // Determine the axis of movement based on the direction
    char axis = axisForDirection(moveDir);

    // Get the entity's current position and the positions of the current and next tiles
    Point3D entityDirection = this->getOrigin();
    Point3D originCurrent = entityCenteredOrigin(currentTile);
    Point3D originNext = entityCenteredOrigin(tileNext);

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

bool MovableEntity::preciseMoveToNextTile(MoveDir moveDir, float frametimeS, bool& moved, bool& inCenter, const std::vector<Tile*>& intersectingTiles) {
    moved = false;
    inCenter = false;

    // Determine the current tile the entity is on
    Tile* tileCurrent = currentTile(intersectingTiles);
    assert(tileCurrent && tileCurrent->isWalkable());

    // Identify the tile in the current movement direction
    Tile* tileToBeHit = nextTileInDirection(moveDir, tileCurrent);
    bool headingOut = headingOutOfMap(moveDir, tileCurrent);

    if (!headingOut && (!tileToBeHit || !tileToBeHit->isWalkable())) {
        // Abort movement if next tile is invalid or not walkable
        return false;
    }

    // Get movement axis and adjust speed for current direction
    float speed = frametimeNormalizedSpeed(frametimeS) * speedMltprForDirection(moveDir);
    char axis = axisForDirection(moveDir);

    if (headingOut) {
        if (preciseMove(moveDir, frametimeS, moved));
        return true;
    }

    // Move toward the next closest tile; set canTurn if destination reached
    if (tryMoveToNextClosestTile(moveDir, this, axis, speed, inCenter, moved, true)) {
        return true;
    }

    return false;
}

Point3D MovableEntity::entityCenteredOrigin(const Tile* tile) const {
    Point3D tileAlignedOrigin = Point3D(tile->getOrigin());
    tileAlignedOrigin.move((MapFactory::TILE_SIZE - this->boundingBox.getSizeX()) / 2, 0, (MapFactory::TILE_SIZE - this->boundingBox.getSizeY()) / 2);
    return tileAlignedOrigin;
}

// === Tile Utility Functions ===
bool MovableEntity::areTilesWalkable(std::vector<Tile*> tiles) {
    if (tiles.empty()) { return false; }
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
    BoundingBox3D absBbox = movableEntity->getAbsoluteBoundingBox();
    std::vector<Tile*> intersectedTiles = movableEntity->map->getTilesWithBoundingBox(&absBbox);
    return intersectedTiles;
}

Tile* MovableEntity::closestTile(const MovableEntity* movableEntity, const std::vector<Tile*>& tiles, float& distance) {
    if (tiles.empty()) { return nullptr; }
    Point3D entityOrigin = movableEntity->getOrigin();

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

float MovableEntity::clampMoveDistance(float moveDistance, float maxDistance) {
    ASSERT_MSG(maxDistance > 0, "maxDistance must be a positive number!");

    float clampedValue = std::min(std::abs(moveDistance), maxDistance);
    return moveDistance < 0 ? -clampedValue : clampedValue;
}