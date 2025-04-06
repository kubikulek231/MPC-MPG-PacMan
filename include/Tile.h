#ifndef TILE_H
#define TILE_H

#include <vector>
#include <string>
#include "Entity.h"

enum class TileType {
    EMPTY = 1,   // Empty space
    WALL = 2,    // Wall
    PELLET = 3,  // Pellet
};

class Tile : public Entity {
public:
    Tile(TileType tileType, Point3D tileOrigin, BoundingBox3D tileBoundingBox);
    bool isWalkable() const;
    void setType(TileType tileType);
    std::string getTileTypeString();
    TileType getTileType() const;
    Point3D getCenterPoint() const;
    float distanceToCenter(const Tile& other) const;

private:
    TileType tileType;
};

#endif
