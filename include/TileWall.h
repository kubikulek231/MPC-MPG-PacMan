#ifndef TILEWALL_H
#define TILEWALL_H

#include "gl_includes.h"
#include "Tile.h"

enum class WallType {
    BLOCK = 0,              // Full wall block
    LEFT = 1,               // Left side wall
    RIGHT = 2,              // Right side wall
    TOP = 3,                // Top side wall
    BOTTOM = 4,             // Bottom side wall

    TOP_LEFT_CORNER = 5,    // Outer corner
    TOP_RIGHT_CORNER = 6,
    BOTTOM_LEFT_CORNER = 7,
    BOTTOM_RIGHT_CORNER = 8,

    INNER_TOP_LEFT = 9,     // Inner (concave) corner
    INNER_TOP_RIGHT = 10,
    INNER_BOTTOM_LEFT = 11,
    INNER_BOTTOM_RIGHT = 12,

    NONE = 13               // Fallback/default if needed
};

class TileWall : public Tile {
    // Fraction of the tile that the wall occupies
    static constexpr float WALL_THICKNESS_FRAC = 0.5f;
    // The remaining gap on each side, in fraction of tile
    static constexpr float GAP_FRAC = (1.0f - WALL_THICKNESS_FRAC) * 0.5f;
private:
    WallType wallType = WallType::BLOCK;
    void renderWallBlock() const;
    void renderWallLeft() const;
    void renderWallRight() const;
    void renderWallTop() const;
    void renderWallBottom() const;
    void renderWallCornerTopLeft() const;
    void renderWallCornerTopRight() const;
    void renderWallCornerBottomLeft() const;
    void renderWallCornerBottomRight() const;
    void renderWallInnerTopLeft() const;
    void renderWallInnerTopRight() const;
    void renderWallInnerBottomLeft() const;
    void renderWallInnerBottomRight() const;
    void setWallTypeStraight();
    void setWallTypeOuterCorners();
public:
    TileWall(WallType wallType, TileType tileType, Point3D tileOrigin, BoundingBox3D tileBoundingBox, int tileRow, int tileCol);
    ~TileWall() override = default;
    void render() const override;
    void setWallType(WallType wallType) { this->wallType = wallType; }
    // Sets WallType according to neighbor tiles
    void setWallTypeByNeighbors();
};

#endif
