#ifndef MAP_H
#define MAP_H

#include "gl_includes.h"
#include <vector>
#include "Tile.h"
#include "Point3D.h"
#include "BoundingBox3D.h"
#include "Map.h"
#include <memory>

struct MapCornerPoints {
    Point3D lowerLeft = Point3D();
    Point3D upperLeft = Point3D();
    Point3D lowerRight = Point3D();
    Point3D upperRight = Point3D();
};

enum class MapCorner {
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT
};

class Map {
public:
    static const std::vector<MapCorner> corners;
    Map();
    Map(const std::vector<std::vector<std::shared_ptr<Tile>>>& mapGrid, float tileSize, int totalPellets);
    void render(bool resetHighlighted = false, int resetTimerMs = 5000);  // Draws the map
    Tile* getTileWithPoint3D(Point3D point);
    Tile* getTileAt(int row, int col);
    std::vector<Tile*> getTilesWithBoundingBox(BoundingBox3D* boundingBox);
    void resetHighlightedTiles();
    void scheduleHighlightReset(int delay);
    Tile* getRandomTile();
    bool areAllPelletsCollected() const;
    bool collectPellet(Tile* tile);
    Tile* getPlayerSpawn();
    Tile* getBlinkySpawn();
    Tile* getPinkySpawn();
    Tile* getInkySpawn();
    Tile* getClydeSpawn();
    MapCornerPoints getMapCornerPoints() const { return mapCornerPoints; }
private:
    Tile* getFirstTileOfType(TileType type);
    int totalPellets;
    std::vector<std::vector<std::shared_ptr<Tile>>> grid;
    int width;
    int height;
    float tileSize;
    bool isHighlightResetScheduled = false;
    void renderWorldCoordinates(const Tile* tile);
    void renderTileCoordinates(const Tile* tile);
    void drawCenterAxes(float length = 2.0f);
    int mapCollectedPellets = 0;
    MapCornerPoints mapCornerPoints;
};

#endif // MAP_H
