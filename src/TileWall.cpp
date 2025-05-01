#include "TileWall.h"
#include "MapFactory.h"

TileWall::TileWall(WallType wallType,
                   TileType tileType, 
                   Point3D tileOrigin, 
                   BoundingBox3D tileBoundingBox,
                   int tileRow, 
                   int tileCol) 
                   : Tile(
                       tileType, 
                       tileOrigin, 
                       tileBoundingBox, 
                       tileRow, 
                       tileCol)
{
    this->wallType = wallType;
}

// Public method
void TileWall::render() const {
    switch (wallType) {
    case WallType::BLOCK:
        renderWallBlock();
        break;
    case WallType::LEFT:
        renderWallLeft();
        break;
    case WallType::RIGHT:
        renderWallRight();
        break;
    case WallType::TOP:
        renderWallTop();
        break;
    case WallType::BOTTOM:
        renderWallBottom();
        break;
    case WallType::TOP_LEFT_CORNER:
        renderWallCornerTopLeft();
        break;
    case WallType::TOP_RIGHT_CORNER:
        renderWallCornerTopRight();
        break;
    case WallType::BOTTOM_LEFT_CORNER:
        renderWallCornerBottomLeft();
        break;
    case WallType::BOTTOM_RIGHT_CORNER:
        renderWallCornerBottomRight();
        break;
    case WallType::INNER_TOP_LEFT:
        renderWallInnerTopLeft();
        break;
    case WallType::INNER_TOP_RIGHT:
        renderWallInnerTopRight();
        break;
    case WallType::INNER_BOTTOM_LEFT:
        renderWallInnerBottomLeft();
        break;
    case WallType::INNER_BOTTOM_RIGHT:
        renderWallInnerBottomRight();
        break;
    case WallType::NONE:
    default:
        break;
    }
}

void TileWall::renderWallBlock() const {
    BoundingBox3D abb = this->getAbsoluteBoundingBox();
    glColor3f(0.3f, 0.3f, 1.0f); // Blue for wall

    float centerX = (abb.min.x + abb.max.x) / 2.0f;
    float centerY = (abb.min.y + abb.max.y) / 2.0f;
    float centerZ = (abb.min.z + abb.max.z) / 2.0f;

    glPushMatrix();
    glTranslatef(centerX, centerY, centerZ);
    glutSolidCube(MapFactory::TILE_SIZE);
    glPopMatrix();
}

void TileWall::renderWallLeft() const {
    BoundingBox3D abb = this->getAbsoluteBoundingBox();
    glColor3f(0.3f, 0.3f, 1.0f); // Blue for wall

    float width = MapFactory::TILE_SIZE * 0.5f; // Half-width
    float height = MapFactory::TILE_SIZE;
    float centerX = (abb.min.x + abb.max.x) / 2.0f - width / 2.0f;
    float centerY = (abb.min.y + abb.max.y) / 2.0f;
    float centerZ = (abb.min.z + abb.max.z) / 2.0f;

    glPushMatrix();
    glTranslatef(centerX, centerY, centerZ);
    glScalef(0.5f, 1.0f, 1.0f); // Shrink along X
    glutSolidCube(MapFactory::TILE_SIZE);
    glPopMatrix();
}

void TileWall::renderWallRight() const {
    BoundingBox3D abb = this->getAbsoluteBoundingBox();
    glColor3f(0.3f, 0.3f, 1.0f);

    float width = MapFactory::TILE_SIZE * 0.5f;
    float centerX = (abb.min.x + abb.max.x) / 2.0f + width / 2.0f;
    float centerY = (abb.min.y + abb.max.y) / 2.0f;
    float centerZ = (abb.min.z + abb.max.z) / 2.0f;

    glPushMatrix();
    glTranslatef(centerX, centerY, centerZ);
    glScalef(0.5f, 1.0f, 1.0f);
    glutSolidCube(MapFactory::TILE_SIZE);
    glPopMatrix();
}

void TileWall::renderWallTop() const {
    BoundingBox3D abb = this->getAbsoluteBoundingBox();
    glColor3f(0.3f, 0.3f, 1.0f);

    float depth = MapFactory::TILE_SIZE * 0.5f;
    float centerX = (abb.min.x + abb.max.x) / 2.0f;
    float centerY = (abb.min.y + abb.max.y) / 2.0f;
    float centerZ = (abb.min.z + abb.max.z) / 2.0f + depth / 2.0f;

    glPushMatrix();
    glTranslatef(centerX, centerY, centerZ);
    glScalef(1.0f, 1.0f, 0.5f);
    glutSolidCube(MapFactory::TILE_SIZE);
    glPopMatrix();
}

void TileWall::renderWallBottom() const {
    BoundingBox3D abb = this->getAbsoluteBoundingBox();
    glColor3f(0.3f, 0.3f, 1.0f);

    float depth = MapFactory::TILE_SIZE * 0.5f;
    float centerX = (abb.min.x + abb.max.x) / 2.0f;
    float centerY = (abb.min.y + abb.max.y) / 2.0f;
    float centerZ = (abb.min.z + abb.max.z) / 2.0f - depth / 2.0f;

    glPushMatrix();
    glTranslatef(centerX, centerY, centerZ);
    glScalef(1.0f, 1.0f, 0.5f);
    glutSolidCube(MapFactory::TILE_SIZE);
    glPopMatrix();
}

void TileWall::renderWallCornerTopLeft() const {
    BoundingBox3D abb = this->getAbsoluteBoundingBox();
    glColor3f(0.3f, 0.3f, 1.0f);

    float offset = MapFactory::TILE_SIZE * 0.25f;
    float centerX = (abb.min.x + abb.max.x) / 2.0f - offset;
    float centerY = (abb.min.y + abb.max.y) / 2.0f;
    float centerZ = (abb.min.z + abb.max.z) / 2.0f + offset;

    glPushMatrix();
    glTranslatef(centerX, centerY, centerZ);
    glScalef(0.5f, 1.0f, 0.5f);
    glutSolidCube(MapFactory::TILE_SIZE);
    glPopMatrix();
}

void TileWall::renderWallCornerTopRight() const {
    BoundingBox3D abb = this->getAbsoluteBoundingBox();
    glColor3f(0.3f, 0.3f, 1.0f);

    float offset = MapFactory::TILE_SIZE * 0.25f;
    float centerX = (abb.min.x + abb.max.x) / 2.0f + offset;
    float centerY = (abb.min.y + abb.max.y) / 2.0f;
    float centerZ = (abb.min.z + abb.max.z) / 2.0f + offset;

    glPushMatrix();
    glTranslatef(centerX, centerY, centerZ);
    glScalef(0.5f, 1.0f, 0.5f);
    glutSolidCube(MapFactory::TILE_SIZE);
    glPopMatrix();
}

void TileWall::renderWallCornerBottomLeft() const {
    BoundingBox3D abb = this->getAbsoluteBoundingBox();
    glColor3f(0.3f, 0.3f, 1.0f);

    float offset = MapFactory::TILE_SIZE * 0.25f;
    float centerX = (abb.min.x + abb.max.x) / 2.0f - offset;
    float centerY = (abb.min.y + abb.max.y) / 2.0f;
    float centerZ = (abb.min.z + abb.max.z) / 2.0f - offset;

    glPushMatrix();
    glTranslatef(centerX, centerY, centerZ);
    glScalef(0.5f, 1.0f, 0.5f);
    glutSolidCube(MapFactory::TILE_SIZE);
    glPopMatrix();
}

void TileWall::renderWallCornerBottomRight() const {
    BoundingBox3D abb = this->getAbsoluteBoundingBox();
    glColor3f(0.3f, 0.3f, 1.0f);

    float offset = MapFactory::TILE_SIZE * 0.25f;
    float centerX = (abb.min.x + abb.max.x) / 2.0f + offset;
    float centerY = (abb.min.y + abb.max.y) / 2.0f;
    float centerZ = (abb.min.z + abb.max.z) / 2.0f - offset;

    glPushMatrix();
    glTranslatef(centerX, centerY, centerZ);
    glScalef(0.5f, 1.0f, 0.5f);
    glutSolidCube(MapFactory::TILE_SIZE);
    glPopMatrix();
}

void TileWall::renderWallInnerTopLeft() const {
    BoundingBox3D abb = this->getAbsoluteBoundingBox();
    glColor3f(0.2f, 0.2f, 0.8f); // Slightly different shade

    float offset = MapFactory::TILE_SIZE * 0.25f;
    float centerX = (abb.min.x + abb.max.x) / 2.0f + offset;
    float centerY = (abb.min.y + abb.max.y) / 2.0f;
    float centerZ = (abb.min.z + abb.max.z) / 2.0f - offset;

    glPushMatrix();
    glTranslatef(centerX, centerY, centerZ);
    glScalef(0.25f, 1.0f, 0.25f);
    glutSolidCube(MapFactory::TILE_SIZE);
    glPopMatrix();
}

void TileWall::renderWallInnerTopRight() const {
    BoundingBox3D abb = this->getAbsoluteBoundingBox();
    glColor3f(0.2f, 0.2f, 0.8f);

    float offset = MapFactory::TILE_SIZE * 0.25f;
    float centerX = (abb.min.x + abb.max.x) / 2.0f - offset;
    float centerY = (abb.min.y + abb.max.y) / 2.0f;
    float centerZ = (abb.min.z + abb.max.z) / 2.0f - offset;

    glPushMatrix();
    glTranslatef(centerX, centerY, centerZ);
    glScalef(0.25f, 1.0f, 0.25f);
    glutSolidCube(MapFactory::TILE_SIZE);
    glPopMatrix();
}

void TileWall::renderWallInnerBottomLeft() const {
    BoundingBox3D abb = this->getAbsoluteBoundingBox();
    glColor3f(0.2f, 0.2f, 0.8f);

    float offset = MapFactory::TILE_SIZE * 0.25f;
    float centerX = (abb.min.x + abb.max.x) / 2.0f + offset;
    float centerY = (abb.min.y + abb.max.y) / 2.0f;
    float centerZ = (abb.min.z + abb.max.z) / 2.0f + offset;

    glPushMatrix();
    glTranslatef(centerX, centerY, centerZ);
    glScalef(0.25f, 1.0f, 0.25f);
    glutSolidCube(MapFactory::TILE_SIZE);
    glPopMatrix();
}

void TileWall::renderWallInnerBottomRight() const {
    BoundingBox3D abb = this->getAbsoluteBoundingBox();
    glColor3f(0.2f, 0.2f, 0.8f);

    float offset = MapFactory::TILE_SIZE * 0.25f;
    float centerX = (abb.min.x + abb.max.x) / 2.0f - offset;
    float centerY = (abb.min.y + abb.max.y) / 2.0f;
    float centerZ = (abb.min.z + abb.max.z) / 2.0f + offset;

    glPushMatrix();
    glTranslatef(centerX, centerY, centerZ);
    glScalef(0.25f, 1.0f, 0.25f);
    glutSolidCube(MapFactory::TILE_SIZE);
    glPopMatrix();
}