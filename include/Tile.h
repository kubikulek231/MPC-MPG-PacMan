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
    Tile(TileType tileType, Point3D tileOrigin, BoundingBox3D tileBoundingBox, int tileRow, int tileCol);
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
    void renderHighlight() const;

    // Getter and Setter for neighboring tiles
    Tile* getTileUp() const;
    Tile* getTileDown() const;
    Tile* getTileLeft() const;
    Tile* getTileRight() const;

    int getTileRow() const;
    int getTileCol() const;

    void setTileUp(Tile* tile);
    void setTileDown(Tile* tile);
    void setTileLeft(Tile* tile);
    void setTileRight(Tile* tile);

    void setHighlight(bool value);
    void setHighlightColor(float r = 1.0, float g = 0.0, float b = 0.0, float a = 0.1);
    std::string toString();

    bool operator==(const Tile& other) const {
        return (this->tileRow == other.tileRow) && (this->tileCol == other.tileCol);
    }
private:
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
