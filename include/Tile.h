#ifndef TILE_H
#define TILE_H

#include <vector>

enum class TileType {
    EMPTY = 1,  // Empty space
    WALL = 2,   // Wall
    PELLET = 3,  // Pellet
};

class Tile {
public:
    Tile(TileType tileType, float xPosMin, float yPosMin, float xPosMax, float yPosMax);
    bool isWalkable();
    void setType(TileType tileType);
    float getCenterX();
    float getCenterY();
    TileType getTileType();
private:
    TileType tileType;
    float xPosMin, yPosMin, xPosMax, yPosMax, xCenterPos, yCenterPos;
};

#endif
