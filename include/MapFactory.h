#ifndef MAP_FACTORY_H
#define MAP_FACTORY_H

#include "Map.h"
#include <string>

class MapFactory {
public:
    MapFactory();
    Map createMap();
    static const int MAP_HEIGHT = 36;
    static const int MAP_WIDTH = 28;
    static constexpr float MAP_Y = 0.0f;
    static constexpr float TILE_SIZE = 1.0f;

private:
    std::vector<std::vector<Tile>> grid;
    bool isValidCoord(int x, int y);
    bool loadMapFile(const std::string& filename);
    void generateEmptyMapFile(const std::string& filename);
    void createDefaultGrid();
    void setTileNeighbors();
};

#endif // MAP_FACTORY_H
