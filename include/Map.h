#ifndef MAP_H
#define MAP_H

#include <vector>
#include "Tile.h"

class Map {
public:
    Map();
    Map(std::vector<std::vector<Tile>> mapGrid, float tileSize);
    void render();  // Draws the map
    Tile* getTile(float posX, float posY);
private:
    std::vector<std::vector<Tile>> grid;
    int width;
    int height;
    float tileSize;
};

#endif // MAP_H
