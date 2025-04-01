#ifndef MAP_FACTORY_H
#define MAP_FACTORY_H

#include "Map.h"
#include <string>

class MapFactory {
public:
    MapFactory();
    Map createMap();
    static const int MAP_HEIGHT = 31;
    static const int MAP_WIDTH = 28;

private:
    std::vector<std::vector<TileType>> grid;
    bool isValidCoord(int x, int y);
    bool canNewBlockFit(int x, int y);
    void addWallBlock(int x, int y);
    void expandWall(int x, int y);

    bool loadMapFile(const std::string& filename);
    void generateEmptyMapFile(const std::string& filename);
    void createGhostHouse();
    void createWalls();
    void generateMaze(int startX, int startY);
    void createGeneratedWalls();
    void createPellets();
};

#endif // MAP_FACTORY_H
