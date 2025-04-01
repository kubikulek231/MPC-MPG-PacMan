#ifndef MAP_H
#define MAP_H

#include <vector>

enum class TileType {
    EMPTY  = 0,  // Empty space
    WALL   = 1,   // Wall
    PELLET = 2,  // Pellet
};

class Map {
public:
    static constexpr double TILE_SIZE = 1.0f;
    Map();
    Map(std::vector<std::vector<TileType>> mapGrid);
    void render();  // Draws the map
    std::vector<std::vector<TileType>> grid;
    int width;
    int height;
};

#endif // MAP_H
