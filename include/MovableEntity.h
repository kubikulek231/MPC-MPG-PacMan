#ifndef MOVABLEENTITY_H
#define MOVABLEENTITY_H

#include "gl_includes.h"
#include <vector>
#include "Entity.h"
#include "MoveDir.h"
#include "Tile.h"
#include "DirChangeRequest.h"
#include "Map.h"
#include "MapFactory.h"

// Represents an entity that can move on a tile-based map.
class MovableEntity : public Entity {
public:
    // === Constants ===
    static constexpr float DEFAULT_SPEED = 1.0f;
    static constexpr float DEFAULT_SNAP_DISTANCE = 0.05f;
    static const MoveDir DEFAULT_MOVE_DIR;
    static constexpr bool DEFAULT_DIR_CHANGE_REQUEST_EXPIRE = false;
    static constexpr uint64_t DEFAULT_DIR_CHANGE_REQUEST_EXPIRE_AFTER_MS = 1000;
    static constexpr float MAX_NORMALIZED_SPEED = MapFactory::TILE_SIZE / 2.0f;

    // === Constructors ===
    MovableEntity() = default;
    MovableEntity(Map* map,
        Point3D origin,
        BoundingBox3D boundingBox,
        MoveDir moveDir = DEFAULT_MOVE_DIR,
        float speed = DEFAULT_SPEED,
        float snapDistance = DEFAULT_SNAP_DISTANCE,
        bool dirChangeRequestExpire = DEFAULT_DIR_CHANGE_REQUEST_EXPIRE,
        uint64_t dirChangeRequestExpireAfterMs = DEFAULT_DIR_CHANGE_REQUEST_EXPIRE_AFTER_MS);
    MovableEntity(const MovableEntity& other);

    // === Movement Interface ===
    bool move(MoveDir requestedMoveDir, bool& isNewRequest, float frameTimeMs);
    void setMoveSpeed(float speed);
    float getMoveSpeed() const;
    void setDirChangeRequestExpireAfterMs(uint64_t expireAfter) { this->dirChangeRequestExpireAfterMs = expireAfter; }

protected:
    // === State ===
    Map* map = nullptr;
    DirChangeRequest* dirChangeRequest = nullptr;
    MoveDir moveDir = MoveDir::UNDEFINED;
    float speed = DEFAULT_SPEED;
    float snapDistance = DEFAULT_SNAP_DISTANCE;
    bool dirChangeRequestExpire = DEFAULT_DIR_CHANGE_REQUEST_EXPIRE;
    uint64_t dirChangeRequestExpireAfterMs = DEFAULT_DIR_CHANGE_REQUEST_EXPIRE_AFTER_MS;

    // === Direction Change Management ===
    void createDirChangeRequest(MoveDir moveDir, float expireAfterMs, bool expire = true);
    void clearDirChangeRequest();
    MoveDir getRequestedDir();

    // === Frame-based Speed Calculation ===
    float frametimeNormalizedSpeed(float frameTimeMs) const;

    // === Low-Level Movement Helpers ===
    void move(float dx, float dy, float dz);
    void move(Point3D dPoint);
    void moveX(float dx);
    void moveY(float dy);
    void moveZ(float dz);
    void moveAxis(char axis = 'x', float dAxis = 1.0f);

    // === Direction & Speed Helpers ===
    char axisForDirection(MoveDir moveDir);
    float speedMltprForDirection(MoveDir moveDir);

    // === Movement Logic ===
    bool preciseMove(MoveDir moveDir, float frameTimeMs, bool &moved);
    bool preciseMoveUntilCanTurn(MoveDir actualMoveDir, float frameTimeMs, bool& canTurn, bool& moved, const std::vector<Tile*>& intersectingTiles = {});
    bool preciseMoveToNextTile(MoveDir moveDir, float frameTimeMs, bool& moved, bool& inCenter, const std::vector<Tile*>& intersectingTiles);
    bool tryMoveToNextClosestTile(MoveDir moveDir, MovableEntity* movableEntity, char axis, float maxMoveDistance, bool& hit, bool& moved, bool proceedToNextTile = false);
    void teleport(MoveDir moveDir);
    Point3D furthestPossibleEntityOriginPoint(MoveDir moveDir, Tile* currentTile);
    bool headingOutOfMap(MoveDir moveDir, Tile* currentTile);

    // === Tile Navigation & Positioning ===
    Tile* currentTile(const std::vector<Tile*>& intersectingTiles = {}) const;
    Tile* nextTile(MoveDir moveDir, Tile* currentTile);
    Tile* nextTileInDirection(MoveDir moveDir, Tile* currentTile);
    MoveDir dirToTile(Tile* fromTile, Tile* toTile);
    Point3D entityCenteredOrigin(const Tile* movableEntity) const;

    // === Tile Utility Functions ===
    static bool areTilesWalkable(std::vector<Tile*> tiles);
    static std::vector<Tile*> intersectingTiles(const MovableEntity* movableEntity);
    static Tile* closestTile(const MovableEntity* movableEntity, const std::vector<Tile*>& tiles, float& distance);
    static float clampMoveDistance(float moveDistance, float maxDistance);
};

#endif
