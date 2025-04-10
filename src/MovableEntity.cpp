#include "MovableEntity.h"
#include <iostream>
#include <algorithm>
#include <math.h>
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


void MovableEntity::move(MoveDir requestedMoveDir, bool& isNewRequest, float frameTimeMs) {
    // If it is actually a new request
    if (isNewRequest) {
        // Delete existing request
        MovableEntity::clearDirChangeRequest();
        // If actual direction and requested direction differ, set new change request
        if (requestedMoveDir != moveDir) {
            createDirChangeRequest(requestedMoveDir, dirChangeRequestExpireAfterMs, dirChangeRequestExpire);
        }
    }
    // Get player tiles for next calculations
    std::vector<Tile*> intersectingTiles = MovableEntity::intersectingTiles(this);

    // If the same direction as before, just keep going
    if (requestedMoveDir == moveDir) {
        MovableEntity::clearDirChangeRequest();
        MovableEntity::tryMove(moveDir, frameTimeMs, true, intersectingTiles);
        return;
    }

    // Try to follow a pending direction change
    if (dirChangeRequest != nullptr && dirChangeRequest->isPending()) {
        MoveDir requestedDir = dirChangeRequest->getRequestedMoveDir();
        if (MovableEntity::tryMove(requestedDir, frameTimeMs, true, intersectingTiles)) {
            moveDir = requestedDir;
            return;
        }
    }
    // If every attempt fails, keep moving in the same direction
    MovableEntity::tryMove(moveDir, frameTimeMs, true, intersectingTiles);
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

    if (distance > snapDistance) {
        return false;
    }

    // Snap and try again
    copy = *this;
    copy.setOrigin(closestTileOrigin);
    copy = movedCopy(moveDir, moveSpeed);
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

    // Compensate for MovableEntity bounding box sizes
    float entitySizeX = bbox.max.x - bbox.min.x;
    float entitySizeZ = bbox.max.z - bbox.min.z;

    float offsetX = (MapFactory::TILE_SIZE - entitySizeX) / 2.0f;
    float offsetZ = (MapFactory::TILE_SIZE - entitySizeZ) / 2.0f;

    Point3D tileOrigin = closestTile->getOrigin();

    return Point3D(
        tileOrigin.x - offsetX,
        tileOrigin.y,
        tileOrigin.z - offsetZ
    );
}

void MovableEntity::nudgeToTileCenterAxis(float frameTimeMs) {
    // Get the current tile the entity is on
    std::vector<Tile*> intersecting = MovableEntity::intersectingTiles(this);
    if (intersecting.empty()) return;  // No tile, exit early

    float nudgeSpeed = getFrametimeNormalizedSpeed(frameTimeMs); // Normalize speed for the frame time

    // Find the closest tile's center position
    float closestDist = FLT_MAX;
    Point3D target = MovableEntity::closestTileOrigin(this, intersecting, closestDist);
    Point3D current = getOrigin();

    // Move on the axis perpendicular to movement (e.g., X for FWD/BWD, Z for LEFT/RIGHT)
    if (moveDir == MoveDir::FWD || moveDir == MoveDir::BWD) {
        float dx = target.x - current.x;
        if (std::abs(dx) > 0.01f) { // Avoid small threshold issues
            // Nudge towards the tile center, using the bounding box size
            float nudgeX = std::min(std::max(dx, -nudgeSpeed), nudgeSpeed);
            moveX(nudgeX);  // Move along X axis
        }
    }
    else if (moveDir == MoveDir::LEFT || moveDir == MoveDir::RIGHT) {
        float dz = target.z - current.z;
        if (std::abs(dz) > 0.01f) { // Avoid small threshold issues
            // Nudge towards the tile center, using the bounding box size
            float nudgeZ = std::min(std::max(dz, -nudgeSpeed), nudgeSpeed);
            moveZ(nudgeZ);  // Move along Z axis
        }
    }
}
