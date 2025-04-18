#ifndef MAP_H
#define MAP_H

#include <vector>
#include "Tile.h"
#include "Point3D.h"
#include "BoundingBox3D.h"

class Map {
public:
    Map();
    Map(std::vector<std::vector<Tile>> mapGrid, float tileSize);
    void render(bool resetHighlighted = false, int resetTimerMs = 5000);  // Draws the map
    Tile* getTileWithPoint3D(Point3D point);
    Tile* getTileAt(int row, int col);
    std::vector<Tile*> Map::getTilesWithBoundingBox(BoundingBox3D* boundingBox);
    void resetHighlightedTiles();
    void scheduleHighlightReset(int delay);
private:
    std::vector<std::vector<Tile>> grid;
    int width;
    int height;
    float tileSize;
    bool isHighlightResetScheduled = false;
    void renderWorldCoordinates(const Tile* tile);
    void renderTileCoordinates(const Tile* tile);
};

#endif // MAP_H
