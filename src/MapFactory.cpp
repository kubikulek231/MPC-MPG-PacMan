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
    generateEmptyMapFile("C:\\Users\\Kuba\\projects\\MPC-MPG-PacMan\\assets\\maps\\layout.map");
    loadMapFile("C:\\Users\\Kuba\\projects\\MPC-MPG-PacMan\\assets\\maps\\1.map");
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

void MapFactory::createGhostHouse() {
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
                grid[y][x].setType(TileType::WALL); // Set as ghost house wall
            }
            else {
                grid[y][x].setType(TileType::WALL); // Set as empty space inside the ghost house
            }
        }
    }

    // Create a "port" (gap) in the house (e.g., opening on one side of the house)
    // Example: Leaving the left side open
    for (int y = centerY - houseSize / 2 + 1; y < centerY + houseSize / 2 - 1; y++) {
        grid[y][centerX - houseSize / 2].setType(TileType::EMPTY); // Leave one column open on the left side
    }
}

void MapFactory::createWalls() {
    // Iterate over each tile in the grid to create walls around the border
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            // Set walls on the borders
            if (x == 0 || x == MAP_WIDTH - 1 || y == 0 || y == MAP_HEIGHT - 1) {
                grid[y][x].setType(TileType::WALL);
            }
        }
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
            }
            else if (tile == '*') {
                grid[row][col].setType(TileType::EMPTY);
            }
            else {
                std::cerr << "Invalid character '" << tile << "' in map at row " << row << ", column " << col << std::endl;
                return false;
            }
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