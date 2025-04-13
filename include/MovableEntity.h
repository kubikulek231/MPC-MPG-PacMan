#ifndef MOVABLEENTITY_H
#define MOVABLEENTITY_H

#include <GL/glut.h>  // For OpenGL rendering
#include <vector>
#include "Entity.h"
#include "MoveDir.h"
#include "Tile.h"
#include "DirChangeRequest.h"
#include "Map.h"

// Guess what this is.
// Holds a pointer to map for tile colision handling.
class MovableEntity : public Entity {
public:
    static constexpr float DEFAULT_SPEED = 1.0f;
    static constexpr float DEFAULT_SNAP_DISTANCE = 0.05f;
    static const MoveDir DEFAULT_MOVE_DIR = MoveDir::NONE;
    static constexpr bool DEFAULT_DIR_CHANGE_REQUEST_EXPIRE = false;
    static constexpr float DEFAULT_DIR_CHANGE_REQUEST_EXPIRE_AFTER_MS = 1000;

    MovableEntity() {};
    MovableEntity(Map* map, 
                  Point3D origin, 
                  BoundingBox3D boundingBox, 
                  MoveDir moveDir = DEFAULT_MOVE_DIR, 
                  float speed = DEFAULT_SPEED,
                  float snapDistance = DEFAULT_SNAP_DISTANCE,
                  bool dirChangeRequestExpire = DEFAULT_DIR_CHANGE_REQUEST_EXPIRE,
                  float dirChangeRequestExpireAfterMs = DEFAULT_DIR_CHANGE_REQUEST_EXPIRE_AFTER_MS);
    MovableEntity(const MovableEntity& other);

    void move(MoveDir requestedMoveDir, bool& isNewRequest, float frameTimeMs);
    void setMoveSpeed(float speed);
    float getMoveSpeed() const;

protected:
    DirChangeRequest* dirChangeRequest = nullptr;
    Map* map = nullptr;
    MoveDir moveDir = MoveDir::NONE;
    float speed = DEFAULT_SPEED;
    float snapDistance = DEFAULT_SNAP_DISTANCE;
    bool dirChangeRequestExpire = DEFAULT_DIR_CHANGE_REQUEST_EXPIRE;
    float dirChangeRequestExpireAfterMs = DEFAULT_DIR_CHANGE_REQUEST_EXPIRE_AFTER_MS;

    void createDirChangeRequest(MoveDir moveDir, float expireAfterMs, bool expire = true);
    void clearDirChangeRequest();

    float getFrametimeNormalizedSpeed(float frameTimeMs) const { return speed * frameTimeMs; }
    MoveDir getRequestedDir();

    void move(float dx, float dy, float dz);
    void move(Point3D dPoint);
    void moveX(float dx);
    void moveY(float dy);
    void moveZ(float dz);
    void moveAxis(char axis = 'x', float dAxis = 1.0f);

    bool tryMove(MoveDir moveDir, float frameTimeMs, bool snapToTile = false, const std::vector<Tile*> &playerTiles = {});
    MovableEntity movedCopy(MoveDir moveDir, float frameTimeMs);

    bool getAxisAndSpeedForDirection(MoveDir moveDir, char& axis, float& speed);
    bool getAxisForDirection(MoveDir moveDir, char& axis);

    bool preciseMove(MoveDir moveDir, float frameTimeMs);
    bool preciseMoveUntilCanTurn(MoveDir actualMoveDir, float frameTimeMs, bool &turned, const std::vector<Tile*>& intersectingTiles = {});
    bool tryMoveToNextClosestTile(MoveDir moveDir, MovableEntity* movableEntity, char axis, float maxMoveDistance, bool& hit);
    bool tryMoveInAxis(MovableEntity* movableEntity, char axis, float distance);

    Tile* nextTile(MoveDir moveDir, Tile* currentTile);
    Tile* nextTileInDirection(MoveDir moveDir, Tile* currentTile);
    Tile* currentTile(const std::vector<Tile*>& intersectingTiles = {}) const;

    Point3D tileCenteredOrigin(const Tile* movableEntity) const;
    static bool areTilesWalkable(std::vector<Tile*> tiles);
    static std::vector<Tile*> intersectingTiles(const MovableEntity* movableEntity);
    static Tile* closestTile(const MovableEntity* movableEntity, const std::vector<Tile*>& tiles, float& distance);
};

#endif