#ifndef TILE_H
#define TILE_H

#include <vector>
#include <string>
#include "Entity.h"

enum class TileType {
    EMPTY = 1,   // Empty space
    WALL = 2,    // Wall
    PELLET = 3,  // Pellet
    GHOST_HOUSE = 4,
    START = 5,
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
   
    void render() const;
    void renderEmpty() const;
    void renderWall() const;
    void renderPellet() const;

    // Getter and Setter for neighboring tiles
    Tile* getTileUp() const;
    Tile* getTileDown() const;
    Tile* getTileLeft() const;
    Tile* getTileRight() const;

    void setTileUp(Tile* tile);
    void setTileDown(Tile* tile);
    void setTileLeft(Tile* tile);
    void setTileRight(Tile* tile);

    void setHighlight(bool value) { this->highlight = value; };
    std::string toString();
private:
    TileType tileType;
    Tile* tileUp;
    Tile* tileDown;
    Tile* tileLeft;
    Tile* tileRight;
    bool highlight = false;

};

#endif
