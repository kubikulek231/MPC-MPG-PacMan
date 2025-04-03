#ifndef MAP_H
#define MAP_H

#include <vector>

enum class TileType {
    NONE   = 0, 
    EMPTY  = 1,  // Empty space
    WALL   = 2,   // Wall
    PELLET = 3,  // Pellet
};

class Map {
public:
    static constexpr double TILE_SIZE = 1.0f;
    Map();
    Map(std::vector<std::vector<TileType>> mapGrid);
    void render();  // Draws the map
    TileType getTile(float posX, float posY);
private:
    std::vector<std::vector<TileType>> grid;
    int width;
    int height;
};

#endif // MAP_H
