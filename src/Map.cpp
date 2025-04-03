#include "Map.h"
#include <GL/glut.h>

Map::Map() {
}

Map::Map(std::vector<std::vector<Tile>> mapGrid, float tileSize) {
    this->grid = mapGrid;
    this->height = mapGrid.size();  // Number of rows
    this->width = (this->height > 0) ? mapGrid[0].size() : 0;  // Number of columns (checking if the grid is not empty)
    this->tileSize = tileSize;
}

Tile* Map::getTile(float posX, float posY) {
    int tileX = (posX + (width * tileSize / 2)) / tileSize;
    int tileY = (posY + (height * tileSize / 2)) / tileSize;

    if (tileX >= 0 && tileX < width && tileY >= 0 && tileY < height) {
        return &grid[tileY][tileX];
    }
    return nullptr;
}

void Map::render() {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float worldX = (x - width / 2.0f) * tileSize;
            float worldZ = (y - height / 2.0f) * tileSize;

            // Get the tile type
            TileType tileType = grid[y][x].getTileType();

            // Render empty tile
            if (tileType == TileType::EMPTY) {
                glColor3f(0.5f, 0.5f, 0.5f); // Dark gray for empty space
                glPushMatrix();
                glTranslatef(worldX, 0.0f, worldZ);
                glBegin(GL_QUADS);
                glVertex3f(-tileSize / 2.0f, 0.0f, -tileSize / 2.0f);
                glVertex3f(tileSize / 2.0f, 0.0f, -tileSize / 2.0f);
                glVertex3f(tileSize / 2.0f, 0.0f, tileSize / 2.0f);
                glVertex3f(-tileSize / 2.0f, 0.0f, tileSize / 2.0f);
                glEnd();
                glPopMatrix();
                continue;
            }

            // Render wall tile
            if (tileType == TileType::WALL) {
                glColor3f(0.3f, 0.3f, 1.0f); // Blue for wall
                glPushMatrix();
                glTranslatef(worldX, tileSize / 2.0f, worldZ);
                glutSolidCube(tileSize); // Scale walls correctly
                glPopMatrix();
                continue;
            }

            // Render pellet tile
            if (tileType == TileType::PELLET) {
                // Dark gray background
                glColor3f(0.5f, 0.5f, 0.5f);
                glPushMatrix();
                glTranslatef(worldX, 0.0f, worldZ); // Place flat on the ground
                glBegin(GL_QUADS); // Draw flat square
                glVertex3f(-tileSize / 2.0f, 0.0f, -tileSize / 2.0f);
                glVertex3f(tileSize / 2.0f, 0.0f, -tileSize / 2.0f);
                glVertex3f(tileSize / 2.0f, 0.0f, tileSize / 2.0f);
                glVertex3f(-tileSize / 2.0f, 0.0f, tileSize / 2.0f);
                glEnd();
                glPopMatrix();

                // Yellow for pellet
                glColor3f(1.0f, 0.5f, 0.0f); // Yellow for pellet
                glPushMatrix();
                glTranslatef(worldX, tileSize / 2.0f, worldZ);
                glutSolidSphere(tileSize / 8.0, 16, 16); // Scale pellets
                glPopMatrix();
                continue;
            }
        }
    }
}
