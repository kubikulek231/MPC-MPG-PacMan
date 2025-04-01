#include "Map.h"
#include <GL/glut.h>

Map::Map() {
}

Map::Map(std::vector<std::vector<TileType>> mapGrid) {
    this->grid = mapGrid;
    this->height = mapGrid.size();  // Number of rows
    this->width = (this->height > 0) ? mapGrid[0].size() : 0;  // Number of columns (checking if the grid is not empty)
}


void Map::render() {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float worldX = (x - width / 2.0f) * TILE_SIZE;
            float worldZ = (y - height / 2.0f) * TILE_SIZE;

            // Set default background color
            if (grid[y][x] == TileType::EMPTY) {
                glColor3f(0.5f, 0.5f, 0.5f); // Dark gray for empty space
                glPushMatrix();
                glTranslatef(worldX, 0.0f, worldZ); // Place flat on the ground
                glBegin(GL_QUADS); // Draw flat square
                glVertex3f(-TILE_SIZE / 2.0f, 0.0f, -TILE_SIZE / 2.0f);
                glVertex3f(TILE_SIZE / 2.0f, 0.0f, -TILE_SIZE / 2.0f);
                glVertex3f(TILE_SIZE / 2.0f, 0.0f, TILE_SIZE / 2.0f);
                glVertex3f(-TILE_SIZE / 2.0f, 0.0f, TILE_SIZE / 2.0f);
                glEnd();
                glPopMatrix();
            }
            else if (grid[y][x] == TileType::WALL) {  // Wall
                glColor3f(0.3f, 0.3f, 1.0f); // Blue for wall
                glPushMatrix();
                glTranslatef(worldX, TILE_SIZE / 2.0f, worldZ);
                glutSolidCube(TILE_SIZE); // Scale walls correctly
                glPopMatrix();
            }
            else if (grid[y][x] == TileType::PELLET) {  // Pellet
                glColor3f(0.5f, 0.5f, 0.5f); // Dark gray background
                glPushMatrix();
                glTranslatef(worldX, 0.0f, worldZ); // Place flat on the ground
                glBegin(GL_QUADS); // Draw flat square
                glVertex3f(-TILE_SIZE / 2.0f, 0.0f, -TILE_SIZE / 2.0f);
                glVertex3f(TILE_SIZE / 2.0f, 0.0f, -TILE_SIZE / 2.0f);
                glVertex3f(TILE_SIZE / 2.0f, 0.0f, TILE_SIZE / 2.0f);
                glVertex3f(-TILE_SIZE / 2.0f, 0.0f, TILE_SIZE / 2.0f);
                glEnd();
                glPopMatrix();

                glColor3f(1.0f, 1.0f, 0.0f); // Yellow for pellet
                glPushMatrix();
                glTranslatef(worldX, TILE_SIZE / 2.0f, worldZ);
                glutSolidSphere(TILE_SIZE / 5.0, 8, 8); // Scale pellets
                glPopMatrix();
            }
        }
    }
}