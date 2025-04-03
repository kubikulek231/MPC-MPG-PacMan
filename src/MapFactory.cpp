#include "MapFactory.h"
#include <vector>
#include <fstream>    // For file I/O
#include <sstream>    // For stringstream to parse lines
#include <iostream>   // For error handling
#include "Tile.h"

MapFactory::MapFactory() {
    // Initialize the grid with the specified dimensions, filled with EMPTY tiles
    createDefaultGrid();
}

Map MapFactory::createMap() {
    generateEmptyMapFile("assets\\maps\\blank.map");
    loadMapFile("assets\\maps\\1.map");
    //createWalls();
    createPellets();
    return Map(grid, TILE_SIZE);
}

void MapFactory::createDefaultGrid() {
    // Initialize the grid with tiles of specific types or positions
    for (int y = 0; y < MAP_HEIGHT; ++y) {  // Loop through rows (height)
        std::vector<Tile> row;  // Temporary vector to hold a row of tiles

        for (int x = 0; x < MAP_WIDTH; ++x) {  // Loop through columns (width)
            // Calculate positions for tile boundaries
            float xPosMin = static_cast<float>(x) * TILE_SIZE;
            float yPosMin = static_cast<float>(y) * TILE_SIZE;
            float xPosMax = static_cast<float>(x + 1) * TILE_SIZE;
            float yPosMax = static_cast<float>(y + 1) * TILE_SIZE;

            // Add a tile to the row
            row.push_back(Tile(TileType::EMPTY, xPosMin, yPosMin, xPosMax, yPosMax));
        }

        // Add the completed row to the grid
        grid.push_back(row);
    }
}

void MapFactory::createPellets() {
    // Fill all non-wall tiles with pellets
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            // Skip already set walls, and fill other tiles with pellets
            if (grid[y][x].getTileType() == TileType::EMPTY) {
                grid[y][x].setType(TileType::PELLET);
            }
        }
    }
}

bool MapFactory::isValidCoord(int x, int y) {
    return x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT;
}

// Load map from a text file
bool MapFactory::loadMapFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    std::string line;
    int row = 0;

    // Read the map from the file line by line
    while (std::getline(file, line) && row < MAP_HEIGHT) {
        // Ensure the line has the correct length
        if (line.length() != MAP_WIDTH) {
            std::cerr << "Invalid line length at row " << row << ". Expected " << MAP_WIDTH << " characters." << std::endl;
            return false;
        }

        // Parse each character and set the corresponding tile in the grid
        for (int col = 0; col < MAP_WIDTH; col++) {
            char tile = line[col];
            if (tile == 'x') {
                grid[row][col].setType(TileType::WALL);
                continue;
            }
            if (tile == '*') {
                grid[row][col].setType(TileType::EMPTY);
                continue;
            }
            std::cerr << "Invalid character '" << tile << "' in map at row " << row << ", column " << col << std::endl;
            return false;
        }
        row++;
    }
    file.close();
    // Ensure the map is the correct size
    if (row != MAP_HEIGHT) {
        std::cerr << "File content does not match expected map height." << std::endl;
        return false;
    }
    return true;
}

// Method to generate a sample map file filled with '*' characters
void MapFactory::generateEmptyMapFile(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }
    // Create a map filled with '*' characters
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            file << '*';  // Write '*' for empty space
        }
        file << '\n';  // New line at the end of each row
    }
    file.close();
    std::cout << "Empty map generated and saved to " << filename << std::endl;
}