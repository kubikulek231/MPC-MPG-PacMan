#ifndef GHOST_H
#define GHOST_H

#include "gl_includes.h"
#include "MovableEntity.h"
#include <unordered_map>
#include <deque>

class Ghost : public MovableEntity {
private:
    static constexpr float DEFAULT_SPEED = 4.0f;
    static constexpr float DEFAULT_SNAP_DISTANCE = 0.05f;
    static const MoveDir DEFAULT_MOVE_DIR = MoveDir::UNDEFINED;
    static constexpr bool DEFAULT_DIR_CHANGE_REQUEST_EXPIRE = false;
    static constexpr float DEFAULT_DIR_CHANGE_REQUEST_EXPIRE_AFTER_MS = 1000;
    MoveDir randomDirection();
    MoveDir randomTurnDirection();
    bool randomBool();
    bool randomBoolWithChance(float chance = 0.2f);

    // Getting the path to tile
    std::deque<Tile*> reconstructPath(std::unordered_map<Tile*, Tile*> cameFrom, Tile* current);
    std::vector<Tile*> getNeighbors(Tile* a);
    float distance(Tile* a, Tile* b);
    float heuristicCost(Tile* a, Tile* b);
    std::deque<Tile*> movePath = {};

    std::deque<Tile*> shortestPathToTile(Tile* tile);
    MoveDir currentDirection = MoveDir::NONE;
    float colorR = 0.0f;
    float colorG = 1.0f;
    float colorB = 0.0f;
    std::string name = "";
public:
    Ghost();
    Ghost(const Ghost& other);
    Ghost(Map* map, Point3D ghostOrigin, BoundingBox3D ghostBoundingBox, std::string name);
    void render();
    void createPathToTile(Tile* tile);
    void moveOnPath(float frameTimeMs);
    void randomMove(float frameTimeMs);
    void moveOnRandomPath(float frameTimeMs);
    void setColor(float r, float g, float b) {
        colorR = r;
        colorG = g;
        colorB = b;
    }
    bool isPathEmpty() { return movePath.empty(); }
    Tile* furthestTileTowardCorner(MapCorner mapCorner);
};

#endif // PLAYER_H
