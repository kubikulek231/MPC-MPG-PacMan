#ifndef MAP_H
#define MAP_H

#include <vector>
#include "Tile.h"
#include "Point3D.h"
#include "BoundingBox3D.h"

enum class MapCorner {
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT
};

class Map {
public:
    Map();
    Map(std::vector<std::vector<Tile>> mapGrid, float tileSize, int totalPellets);
    void render(bool resetHighlighted = false, int resetTimerMs = 5000);  // Draws the map
    Tile* getTileWithPoint3D(Point3D point);
    Tile* getTileAt(int row, int col);
    std::vector<Tile*> Map::getTilesWithBoundingBox(BoundingBox3D* boundingBox);
    void resetHighlightedTiles();
    void scheduleHighlightReset(int delay);
    Tile* getRandomTile();
    std::vector<MapCorner> corners = {
        MapCorner::TOP_LEFT,
        MapCorner::TOP_RIGHT,
        MapCorner::BOTTOM_LEFT,
        MapCorner::BOTTOM_RIGHT
    };
    bool areAllPelletsCollected() const;
    bool collectPellet(Tile* tile);
private:
    int totalPellets;
    std::vector<std::vector<Tile>> grid;
    int width;
    int height;
    float tileSize;
    bool isHighlightResetScheduled = false;
    void renderWorldCoordinates(const Tile* tile);
    void renderTileCoordinates(const Tile* tile);
    void drawCenterAxes(float length = 2.0f);
    int mapCollectedPellets = 0;
};

#endif // MAP_H
