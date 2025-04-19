#include "Map.h"
#include <GL/glut.h>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <iostream>
#include "Game.h"
#include <random>


Map::Map() {
}

Map::Map(std::vector<std::vector<Tile>> mapGrid, float tileSize, int totalPellets) {
    this->grid = mapGrid;
    this->height = mapGrid.size();  // Number of rows
    this->width = (this->height > 0) ? mapGrid[0].size() : 0;  // Number of columns (checking if the grid is not empty)
    this->tileSize = tileSize;
    this->totalPellets = totalPellets;
}

Tile* Map::getTileWithPoint3D(Point3D point) {
    float originX = (-width / 2.0f) * tileSize;
    float originY = (-height / 2.0f) * tileSize;

    int tileX = static_cast<int>(std::floor((point.x - originX) / tileSize));
    int tileY = static_cast<int>(std::floor((point.y - originY) / tileSize));

    if (tileX >= 0 && tileX < width && tileY >= 0 && tileY < height) {
        return &grid[tileY][tileX];
    }
    return nullptr;
}

Tile* Map::getTileAt(int row, int col) {
    if (row >= 0 && row < height && col >= 0 && col < width) {
        return &grid[row][col];
    }
    return nullptr; // Out of bounds
}

// Returns absoluteBoundingBox intersecting tiles 
std::vector<Tile*> Map::getTilesWithBoundingBox(BoundingBox3D* absoluteBoundingBox) {
    std::vector<Tile*> intersectedTiles;

    // Get 2D bounds of the bounding box (XZ plane in OpenGL)
    float minX = absoluteBoundingBox->min.x;
    float maxX = absoluteBoundingBox->max.x;
    float minZ = absoluteBoundingBox->min.z;
    float maxZ = absoluteBoundingBox->max.z;

    // Convert world coordinates to tile indices based on the XZ plane
    int startX = std::max(0, static_cast<int>(std::floor((minX + (width / 2.0f) * tileSize) / tileSize)));
    int endX = std::min(width - 1, static_cast<int>(std::floor((maxX + (width / 2.0f) * tileSize) / tileSize)));
    int startZ = std::max(0, static_cast<int>(std::floor((minZ + (height / 2.0f) * tileSize) / tileSize)));
    int endZ = std::min(height - 1, static_cast<int>(std::floor((maxZ + (height / 2.0f) * tileSize) / tileSize)));

    // Iterate only over potentially intersecting tiles
    for (int z = startZ; z <= endZ; ++z) {
        for (int x = startX; x <= endX; ++x) {
            Tile& tile = grid[z][x];
            BoundingBox3D &tileBox = tile.getAbsoluteBoundingBox();
            if (absoluteBoundingBox->intersects(tileBox)) {
                intersectedTiles.push_back(&tile);

                // Print the coordinates of the intersected tile
                //std::cout << "Intersecting tile at: (" << x << ", " << z << ")\n";
            }
        }
    }

    return intersectedTiles;
}

void Map::render(bool resetHighlighted, int resetTimerMs) {
    // Center marker
    glColor3f(1.0f, 0.0f, 0.0f);
    glPushMatrix();
    glutSolidSphere(0.15f, 16, 16); // Origin marker
    glPopMatrix();
    drawCenterAxes(2.0f);
    if (resetHighlighted) {
        Map::scheduleHighlightReset(resetTimerMs);
    }

    for (const std::vector<Tile>& tileRow : grid) {        
        for (const Tile& tile : tileRow) {
            tile.renderOrigin();
            tile.render();
            renderTileCoordinates(&tile);
        }
    }
}

void Map::renderWorldCoordinates(const Tile* tile) {
    BoundingBox3D abb = tile->getAbsoluteBoundingBox();
    // Render tile coordinate text at center
    float textX = abb.min.x + 0.1f;
    float textY = abb.min.y + 0.01f; // Slightly above floor
    float textZ = abb.min.z + 0.1f;
    std::ostringstream oss;

    oss << "("
        << std::fixed << std::setprecision(2) << "X: " << abb.min.x << ", "
        << std::fixed << std::setprecision(2) << "Z: " << abb.min.z
        << ")";
    std::string coordStr = oss.str();

    glColor3f(1.0f, 1.0f, 1.0f); // White text

    glRasterPos3f(textX, textY, textZ);
    for (char c : coordStr) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, c);
    }
}

void Map::renderTileCoordinates(const Tile* tile) {
    BoundingBox3D abb = tile->getAbsoluteBoundingBox();
    // Render tile coordinate text at center
    float textX = abb.min.x + 0.1f;
    float textY = abb.min.y + 0.01f; // Slightly above floor
    float textZ = abb.min.z + 0.1f;
    std::ostringstream oss;

    oss << "("
        << std::fixed << std::setprecision(2) << "R: " << tile->getTileRow() << ", "
        << std::fixed << std::setprecision(2) << "C: " << tile->getTileCol()
        << ")";
    std::string coordStr = oss.str();

    glColor3f(1.0f, 1.0f, 1.0f); // White text

    glRasterPos3f(textX, textY, textZ);
    for (char c : coordStr) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, c);
    }
}

void Map::resetHighlightedTiles() {
    for (std::vector<Tile>& tileRow : grid) {
        for (Tile& tile : tileRow) {
            tile.setHighlight(false);
        }
    }
}

// Schedule a reset after a specified delay in milliseconds
void Map::scheduleHighlightReset(int delay) {
    if (!isHighlightResetScheduled) {
        glutTimerFunc(delay, [](int value) {
            // Reset the highlight of tiles after the specified time
            Game& game = Game::getInstance();
            game.getMap()->resetHighlightedTiles();
            game.getMap()->isHighlightResetScheduled = false;
            }, 0);
        isHighlightResetScheduled = true;
    }
}

Tile* Map::getRandomTile() {
    if (height == 0 || width == 0) return nullptr;

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> rowDist(0, height - 1);
    std::uniform_int_distribution<> colDist(0, width - 1);

    Tile* tile = nullptr;
    int attempts = 0;
    const int maxAttempts = 100;

    // Optional: Try to pick a walkable tile
    while (attempts < maxAttempts) {
        int row = rowDist(gen);
        int col = colDist(gen);
        tile = &grid[row][col];

        if (tile && tile->isWalkable()) {
            return tile;
        }

        attempts++;
    }

    return nullptr; // Fallback if no walkable tile found after maxAttempts
}


void Map::drawCenterAxes(float length) {
    glLineWidth(2.0f);
    glBegin(GL_LINES);

    // X-axis (Red)
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(length, 0.0f, 0.0f);

    // Y-axis (Green)
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, length, 0.0f);

    // Z-axis (Blue)
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, length);

    glEnd();
    glLineWidth(1.0f); // Reset to default

    // Axis labels
    glColor3f(1.0f, 0.0f, 0.0f); // X label
    glRasterPos3f(length + 0.1f, 0.0f, 0.0f);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'X');

    glColor3f(0.0f, 1.0f, 0.0f); // Y label
    glRasterPos3f(0.0f, length + 0.1f, 0.0f);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'Y');

    glColor3f(0.0f, 0.0f, 1.0f); // Z label
    glRasterPos3f(0.0f, 0.0f, length + 0.1f);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'Z');
}

bool Map::areAllPelletsCollected() const {
    if (totalPellets == mapCollectedPellets) {
        return true;
    }
    return false;
}

bool Map::collectPellet(Tile* tile) {
    if (tile->collectPellet()) {
        mapCollectedPellets++;
        return true;
    }
    return false;
}

Tile* Map::getPlayerSpawn() {
    return getFirstTileOfType(TileType::SPAWN_PLAYER);
}

Tile* Map::getBlinkySpawn() {
    return getFirstTileOfType(TileType::SPAWN_BLINKY);
}

Tile* Map::getPinkySpawn() {
    return getFirstTileOfType(TileType::SPAWN_PINKY);
}

Tile* Map::getInkySpawn() {
    return getFirstTileOfType(TileType::SPAWN_INKY);
}

Tile* Map::getClydeSpawn() {
    return getFirstTileOfType(TileType::SPAWN_CLYDE);
}

// Helper function
Tile* Map::getFirstTileOfType(TileType type) {
    for (int y = 0; y < MapFactory::MAP_HEIGHT; ++y) {
        for (int x = 0; x < MapFactory::MAP_WIDTH; ++x) {
            if (grid[y][x].getTileType() == type) {
                return &grid[y][x];
            }
        }
    }
    return nullptr;  // Not found
}
