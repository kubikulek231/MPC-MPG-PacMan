#include "MapFactory.h"
#include <vector>


Map MapFactory::createMap() {

    std::vector<std::vector<TileType>> grid(MAP_HEIGHT, std::vector<TileType>(MAP_WIDTH, TileType::EMPTY));
    grid = createWalls(grid);
    grid = createPellets(grid);
    grid = createGhostHouse(grid);
    return Map(grid);
}

std::vector<std::vector<TileType>> MapFactory::createGhostHouse(std::vector<std::vector<TileType>> grid) {
    // Calculate the center position
    int centerX = MAP_WIDTH / 2;
    int centerY = MAP_HEIGHT / 2;

    // Define the size of the ghost house (e.g., 5x5 block)
    int houseSize = 5;

    // Create the walls around the ghost house (5x5 border)
    for (int y = centerY - houseSize / 2; y < centerY + houseSize / 2; y++) {
        for (int x = centerX - houseSize / 2; x < centerX + houseSize / 2; x++) {
            // Check if we are on the border of the ghost house (outside walls)
            if (x == centerX - houseSize / 2 || x == centerX + houseSize / 2 - 1 ||
                y == centerY - houseSize / 2 || y == centerY + houseSize / 2 - 1) {
                grid[y][x] = TileType::WALL; // Set as ghost house wall
            }
            else {
                grid[y][x] = TileType::EMPTY; // Set as empty space inside the ghost house
            }
        }
    }

    // Create a "port" (gap) in the house (e.g., opening on one side of the house)
    // Example: Leaving the left side open
    for (int y = centerY - houseSize / 2 + 1; y < centerY + houseSize / 2 - 1; y++) {
        grid[y][centerX - houseSize / 2] = TileType::EMPTY; // Leave one column open on the left side
    }

    return grid;
}

std::vector<std::vector<TileType>> MapFactory::createWalls(std::vector<std::vector<TileType>> grid) {
    // Iterate over each tile in the grid to create walls around the border
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            // Set walls on the borders
            if (x == 0 || x == MAP_WIDTH - 1 || y == 0 || y == MAP_HEIGHT - 1) {
                grid[y][x] = TileType::WALL;
            }
        }
    }
    return grid;
}

std::vector<std::vector<TileType>> MapFactory::createPellets(std::vector<std::vector<TileType>> grid) {
    // Fill all non-wall tiles with pellets
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            // Skip already set walls, and fill other tiles with pellets
            if (grid[y][x] == TileType::EMPTY) {
                grid[y][x] = TileType::PELLET;
            }
        }
    }
    return grid;
}
