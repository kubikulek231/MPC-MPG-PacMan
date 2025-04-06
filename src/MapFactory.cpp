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
    return Map(grid, TILE_SIZE);
}

void MapFactory::createDefaultGrid() {
    // Initialize the grid with tiles of specific types or positions
    for (int y = 0; y < MAP_HEIGHT; ++y) {  // Loop through rows (height)
        std::vector<Tile> row;  // Temporary vector to hold a row of tiles

        for (int x = 0; x < MAP_WIDTH; ++x) {  // Loop through columns (width)
            // Center the grid around (0,0) in world space
            float xPosMin = (x - MAP_WIDTH / 2.0f) * TILE_SIZE;
            float yPosMin = (y - MAP_HEIGHT / 2.0f) * TILE_SIZE;

            // Set the origin of the tile at the bottom-left corner
            Point3D tileOrigin = Point3D(xPosMin, MAP_Z, yPosMin);

            // Define the bounding box relative to the tile's origin
            Point3D bbMin = Point3D(0.0f, 0.0f, 0.0f);  // Minimum of bounding box is the origin
            Point3D bbMax = Point3D(TILE_SIZE, TILE_SIZE, TILE_SIZE);  // Max point of the bounding box is offset by TILE_SIZE

            // Create the bounding box relative to the origin
            BoundingBox3D tileBoundingBox = BoundingBox3D(bbMin, bbMax);

            // Add a tile to the row
            row.push_back(Tile(TileType::EMPTY, tileOrigin, tileBoundingBox));
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

bool MapFactory::loadMapFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    grid.clear(); // Rebuild the grid from scratch
    std::string line;
    int row = 0;

    // Read the map from the file line by line
    while (std::getline(file, line) && row < MAP_HEIGHT) {
        // Ensure the line has the correct width
        if (line.length() != MAP_WIDTH) {
            std::cerr << "Invalid line length at row " << row
                << ". Expected " << MAP_WIDTH << " characters." << std::endl;
            return false;
        }

        std::vector<Tile> tileRow;

        for (int col = 0; col < MAP_WIDTH; col++) {
            char tileChar = line[col];
            TileType type;

            if (tileChar == 'x') type = TileType::WALL;
            else if (tileChar == '*') type = TileType::PELLET;
            else if (tileChar == 'o') type = TileType::EMPTY;
            else {
                std::cerr << "Invalid character '" << tileChar << "' at row "
                    << row << ", column " << col << std::endl;
                return false;
            }

            // Centered tile positions
            float xCentered = (col - MAP_WIDTH / 2.0f) * TILE_SIZE;
            float yCentered = (row - MAP_HEIGHT / 2.0f) * TILE_SIZE;
            float xPosMin = xCentered;
            float yPosMin = yCentered;
            float xPosMax = xCentered + TILE_SIZE;
            float yPosMax = yCentered + TILE_SIZE;

            // Create the origin and bounding box for the tile
            Point3D tileOrigin = Point3D(xPosMin, MAP_Z, yPosMin);
            Point3D bbMin = Point3D(0.0f, 0.0f, 0.0f);  // Min point of bounding box at origin
            Point3D bbMax = Point3D(TILE_SIZE, TILE_SIZE, TILE_SIZE);  // Max point offset by TILE_SIZE
            BoundingBox3D tileBoundingBox = BoundingBox3D(bbMin, bbMax);

            // Add the tile to the row
            tileRow.emplace_back(type, tileOrigin, tileBoundingBox);
        }

        grid.push_back(tileRow);
        row++;
    }

    file.close();

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