#ifndef MAP_FACTORY_H
#define MAP_FACTORY_H

#include "Map.h"

class MapFactory {
public:
    static Map createMap();
    static const int MAP_WIDTH = 31;
    static const int MAP_HEIGHT = 24;

private:
    static std::vector<std::vector<TileType>> createGhostHouse(std::vector<std::vector<TileType>> map);
    static std::vector<std::vector<TileType>> createWalls(std::vector<std::vector<TileType>> map);
    static std::vector<std::vector<TileType>> createPellets(std::vector<std::vector<TileType>> map);
};

#endif // MAP_FACTORY_H
