#include "Map.h"
#include <GL/glut.h>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <iostream>
#include "Game.h"

Map::Map() {
}

Map::Map(std::vector<std::vector<Tile>> mapGrid, float tileSize) {
    this->grid = mapGrid;
    this->height = mapGrid.size();  // Number of rows
    this->width = (this->height > 0) ? mapGrid[0].size() : 0;  // Number of columns (checking if the grid is not empty)
    this->tileSize = tileSize;
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