#include "Map.h"
#include <GL/glut.h>
#include <cmath>
#include <iostream>

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

            if (absoluteBoundingBox->intersects(tile.getAbsoluteBoundingBox())) {
                intersectedTiles.push_back(&tile);

                // Print the coordinates of the intersected tile
                //std::cout << "Intersecting tile at: (" << x << ", " << z << ")\n";
            }
        }
    }

    return intersectedTiles;
}

void Map::render() {
    // Center marker
    glColor3f(1.0f, 0.0f, 0.0f);
    glPushMatrix();
    glutSolidSphere(0.15f, 16, 16); // Origin marker
    glPopMatrix();

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Tile tile = grid[y][x];
            BoundingBox3D abb = tile.getAbsoluteBoundingBox();
            TileType tileType = tile.getTileType();

            // Debug
            //tile.renderBoundingBox();
            tile.renderOrigin();

            // Common base plane for all tile types
            if (tileType == TileType::EMPTY || tileType == TileType::PELLET) {
                glColor3f(0.5f, 0.5f, 0.5f); // Dark gray for floor
                glBegin(GL_QUADS);
                glVertex3f(abb.min.x, abb.min.y, abb.min.z);
                glVertex3f(abb.max.x, abb.min.y, abb.min.z);
                glVertex3f(abb.max.x, abb.min.y, abb.max.z);
                glVertex3f(abb.min.x, abb.min.y, abb.max.z);
                glEnd();
            }

            if (tileType == TileType::WALL) {
                glColor3f(0.3f, 0.3f, 1.0f); // Blue for wall
                float centerX = (abb.min.x + abb.max.x) / 2.0f;
                float centerY = (abb.min.y + abb.max.y) / 2.0f;
                float centerZ = (abb.min.z + abb.max.z) / 2.0f;
                glPushMatrix();
                glTranslatef(centerX, centerY, centerZ);
                glutSolidCube(tileSize); // Assumes cube fits bounding box
                glPopMatrix();
            }

            if (tileType == TileType::PELLET) {
                // Draw pellet
                glColor3f(1.0f, 0.5f, 0.0f); // Orange for pellet
                float centerX = (abb.min.x + abb.max.x) / 2.0f;
                float centerY = abb.min.y + tileSize / 2.0f;
                float centerZ = (abb.min.z + abb.max.z) / 2.0f;
                glPushMatrix();
                glTranslatef(centerX, centerY, centerZ);
                glutSolidSphere(tileSize / 8.0, 16, 16);
                glPopMatrix();
            }

            //// Render tile coordinate text at center
            //float textX = (abb.min.x + abb.max.x) / 2.0f;
            //float textY = abb.min.y + 0.01f; // Slightly above floor
            //float textZ = (abb.min.z + abb.max.z) / 2.0f;

            //std::string coordStr = "(" + std::to_string(x) + "," + std::to_string(y) + ")";
            //glColor3f(1.0f, 1.0f, 1.0f); // White text

            //glRasterPos3f(textX, textY, textZ);
            //for (char c : coordStr) {
            //    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, c);
            //}
        }
    }
}
