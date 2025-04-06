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
    void render();  // Draws the map
    Tile* getTileWithPoint3D(Point3D point);
    std::vector<Tile*> Map::getTilesWithBoundingBox(BoundingBox3D* boundingBox);
private:
    std::vector<std::vector<Tile>> grid;
    int width;
    int height;
    float tileSize;
};

#endif // MAP_H
