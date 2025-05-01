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

void TileWall::setWallTypeByNeighbors() {
    // Get neighboring tiles
    Tile* tu = getTileUp();
    Tile* td = getTileDown();
    Tile* tl = getTileLeft();
    Tile* tr = getTileRight();

    // A side is open if there is a corridor (walkable tile) on that side
    bool openUp = (tu != nullptr) && tu->isWalkable();
    bool openDown = (td != nullptr) && td->isWalkable();
    bool openLeft = (tl != nullptr) && tl->isWalkable();
    bool openRight = (tr != nullptr) && tr->isWalkable();

    // All four sides open -> isolated block
    if (openUp && openDown && openLeft && openRight) {
        wallType = WallType::BLOCK;
        return;
    }

    // Single-sided segments (corridor on one side -> thin wall on the opposite)
    if (openUp && !openDown && !openLeft && !openRight) {
        // Corridor above -> draw top edge only
        wallType = WallType::TOP;
        return;
    }
    if (openDown && !openUp && !openLeft && !openRight) {
        // Corridor below -> draw bottom edge only
        wallType = WallType::BOTTOM;
        return;
    }
    if (openLeft && !openRight && !openUp && !openDown) {
        // Corridor to the left -> draw left edge only
        wallType = WallType::RIGHT;
        return;
    }
    if (openRight && !openLeft && !openUp && !openDown) {
        // Corridor to the right -> draw right edge only
        wallType = WallType::LEFT;
        return;
    }

    // Inner corners (corridors on two adjacent sides -> concave corner)
    if (openUp && openRight && !openDown && !openLeft) {
        // Corridor above and right -> concave at top-right
        wallType = WallType::INNER_BOTTOM_RIGHT;
        return;
    }
    if (openUp && openLeft && !openDown && !openRight) {
        // Corridor above and left -> concave at top-left
        wallType = WallType::INNER_BOTTOM_LEFT;
        return;
    }
    if (openDown && openRight && !openUp && !openLeft) {
        // Corridor below and right -> concave at bottom-right
        wallType = WallType::INNER_TOP_RIGHT;
        return;
    }
    if (openDown && openLeft && !openUp && !openRight) {
        // Corridor below and left -> concave at bottom-left
        wallType = WallType::INNER_TOP_LEFT;
        return;
    }

    // Outer corners (walls on two adjacent sides -> L-shaped corner)
    if (!openUp && !openLeft && openDown && openRight) {
        // Walls above and left -> outer corner at top-left
        wallType = WallType::TOP_LEFT_CORNER;
        return;
    }
    if (!openUp && !openRight && openDown && openLeft) {
        // Walls above and right -> outer corner at top-right
        wallType = WallType::TOP_RIGHT_CORNER;
        return;
    }
    if (!openDown && !openLeft && openUp && openRight) {
        // Walls below and left -> outer corner at bottom-left
        wallType = WallType::BOTTOM_LEFT_CORNER;
        return;
    }
    if (!openDown && !openRight && openUp && openLeft) {
        // Walls below and right -> outer corner at bottom-right
        wallType = WallType::BOTTOM_RIGHT_CORNER;
        return;
    }

    // Fallback to full block
    wallType = WallType::BLOCK;
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