#ifndef TILE_H
#define TILE_H

#include "gl_includes.h"
#include <vector>
#include <string>
#include "Entity.h"
#include "MoveDir.h"

enum class TileType {
    EMPTY = 1,   // Empty space
    WALL = 2,    // Wall
    PELLET = 3,  // Pellet
    GHOST_HOUSE = 4,
    DOOR_OPEN = 5,
    DOOR_CLOSED = 6,
    TELEPORT = 7,
    SPAWN_PLAYER = 8,
    SPAWN_INKY = 9,
    SPAWN_BLINKY = 10,
    SPAWN_PINKY = 11,
    SPAWN_CLYDE = 12,
    EMPTY_UNWALKABLE = 13,
};

class Tile : public Entity {
public:
    static constexpr float DOOR_HEIGHT = 0.5f;

    Tile(TileType tileType, Point3D tileOrigin, BoundingBox3D tileBoundingBox, int tileRow, int tileCol);
    virtual ~Tile() = default;
    bool collectPellet();
    
    bool isWalkable() const;
    std::string getTileTypeString();
    TileType getTileType() const;
    Point3D getCenterPoint() const;
    float distanceToCenter(const Tile& other) const;
   
    virtual void render() const;
    void renderEmpty() const;
    void renderPellet() const;
    void renderDoorOpen() const;
    void renderHighlight() const;

    // Getter and Setter for neighboring tiles
    Tile* getTileUp() const;
    Tile* getTileDown() const;
    Tile* getTileLeft() const;
    Tile* getTileRight() const;

    Tile* getTileInMoveDir(MoveDir moveDir) const;

    int getTileRow() const;
    int getTileCol() const;

    void setTileUp(Tile* tile);
    void setTileDown(Tile* tile);
    void setTileLeft(Tile* tile);
    void setTileRight(Tile* tile);

    void setHighlight(bool value);
    void setHighlightColor(float r = 1.0, float g = 0.0, float b = 0.0, float a = 0.1);
    std::string toString();
    bool isEqual(const Tile* other) const;
    bool isNeighbor(const Tile* other) const;
    bool isGhostHouseTile() const;
private:
    void setTileType(TileType tileType);
    int tileRow;
    int tileCol;
    TileType tileType;
    Tile* tileUp = nullptr;
    Tile* tileDown = nullptr;
    Tile* tileLeft = nullptr;
    Tile* tileRight = nullptr;
    float highlightR = 1.0f;
    float highlightG = 0.0f;
    float highlightB = 0.0f;
    float highlightA = 0.1f;
    bool highlight = false;
};

#endif
