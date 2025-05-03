#ifndef TILEWALL_H
#define TILEWALL_H

#include "gl_includes.h"
#include "Tile.h"
#include "MapFactory.h"
#include "GameLighting.h"

enum class WallType {
    BLOCK = 0,              // Full wall block
    RIGHT = 1,              // Left side wall
    LEFT = 2,               // Right side wall
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
};

class TileWall : public Tile {
public:
    // Fraction of the tile that the wall occupies
    static constexpr float THICKNESS_FRAC = MapFactory::TILE_SIZE / 2.0f;
    // The remaining gap on each side, in fraction of tile
    static constexpr float GAP_FRAC = (MapFactory::TILE_SIZE - THICKNESS_FRAC) * 0.5f;
    static constexpr float INNER_RADIUS_FRAC = 0.3f;
    static constexpr int   CYLINDER_SEGMENTS = 16;

    static constexpr float COLOR[3] = { 0.1f, 0.2f, 0.7f };
    static constexpr GLfloat LIGHT_AMBIENT[4] = { 0.05f, 0.1f, 0.2f, 1.0f };
    static constexpr GLfloat LIGHT_DIFFUSE[4] = { 0.1f, 0.2f, 0.7f, 1.0f };
    static constexpr GLfloat LIGHT_SPECULAR[4] = { 0.2f, 0.4f, 0.6f, 1.0f };
    static constexpr GLfloat LIGHT_EMISSION[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    static constexpr float LIGHT_SHININESS = 128.0f;


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
