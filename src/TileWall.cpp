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
    setWallTypeStraight();
    setWallTypeOuterCorners();
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
    BoundingBox3D abb = getAbsoluteBoundingBox();
    glColor3f(0.3f, 0.3f, 1.0f);

    float halfY = (abb.min.y + abb.max.y) * 0.5f;
    float halfZ = (abb.min.z + abb.max.z) * 0.5f;
    float gap = MapFactory::TILE_SIZE * GAP_FRAC;
    float thickness = MapFactory::TILE_SIZE * WALL_THICKNESS_FRAC;

    // shift left by GAP so the wall hugs the left edge
    float centerX = (abb.min.x + abb.max.x) * 0.5f - gap;

    glPushMatrix();
    glTranslatef(centerX, halfY, halfZ);
    glScalef(WALL_THICKNESS_FRAC, 1.0f, 1.0f);
    glutSolidCube(MapFactory::TILE_SIZE);
    glPopMatrix();
}

void TileWall::renderWallRight() const {
    BoundingBox3D abb = getAbsoluteBoundingBox();
    glColor3f(0.3f, 0.3f, 1.0f);

    float halfY = (abb.min.y + abb.max.y) * 0.5f;
    float halfZ = (abb.min.z + abb.max.z) * 0.5f;
    float gap = MapFactory::TILE_SIZE * GAP_FRAC;

    // shift right by GAP
    float centerX = (abb.min.x + abb.max.x) * 0.5f + gap;

    glPushMatrix();
    glTranslatef(centerX, halfY, halfZ);
    glScalef(WALL_THICKNESS_FRAC, 1.0f, 1.0f);
    glutSolidCube(MapFactory::TILE_SIZE);
    glPopMatrix();
}

void TileWall::renderWallTop() const {
    BoundingBox3D abb = getAbsoluteBoundingBox();
    glColor3f(0.3f, 0.3f, 1.0f);

    float halfX = (abb.min.x + abb.max.x) * 0.5f;
    float halfY = (abb.min.y + abb.max.y) * 0.5f;
    float gap = MapFactory::TILE_SIZE * GAP_FRAC;

    float centerZ = (abb.min.z + abb.max.z) * 0.5f + gap;

    glPushMatrix();
    glTranslatef(halfX, halfY, centerZ);
    glScalef(1.0f, 1.0f, WALL_THICKNESS_FRAC);
    glutSolidCube(MapFactory::TILE_SIZE);
    glPopMatrix();
}

void TileWall::renderWallBottom() const {
    BoundingBox3D abb = getAbsoluteBoundingBox();
    glColor3f(0.3f, 0.3f, 1.0f);

    float halfX = (abb.min.x + abb.max.x) * 0.5f;
    float halfY = (abb.min.y + abb.max.y) * 0.5f;
    float gap = MapFactory::TILE_SIZE * GAP_FRAC;

    float centerZ = (abb.min.z + abb.max.z) * 0.5f - gap;

    glPushMatrix();
    glTranslatef(halfX, halfY, centerZ);
    glScalef(1.0f, 1.0f, WALL_THICKNESS_FRAC);
    glutSolidCube(MapFactory::TILE_SIZE);
    glPopMatrix();
}

void TileWall::renderWallCornerTopLeft() const {
    renderWallTop();
    renderWallRight();
}

void TileWall::renderWallCornerTopRight() const {
    renderWallTop();
    renderWallLeft();
}

void TileWall::renderWallCornerBottomLeft() const {
    renderWallBottom();
    renderWallRight();
}

void TileWall::renderWallCornerBottomRight() const {
    renderWallBottom();
    renderWallLeft();
}

void TileWall::renderWallInnerTopLeft() const {
    BoundingBox3D abb = getAbsoluteBoundingBox();
    glColor3f(0.2f, 0.2f, 0.8f);

    float halfY = (abb.min.y + abb.max.y) * 0.5f;
    float gap = MapFactory::TILE_SIZE * GAP_FRAC;

    // inner corner is also thickness × thickness
    float centerX = ((abb.min.x + abb.max.x) * 0.5f) + gap;
    float centerZ = ((abb.min.z + abb.max.z) * 0.5f) - gap;

    glPushMatrix();
    glTranslatef(centerX, halfY, centerZ);
    glScalef(WALL_THICKNESS_FRAC, 1.0f, WALL_THICKNESS_FRAC);
    glutSolidCube(MapFactory::TILE_SIZE);
    glPopMatrix();
}

void TileWall::renderWallInnerTopRight() const {
    BoundingBox3D abb = getAbsoluteBoundingBox();
    glColor3f(0.2f, 0.2f, 0.8f);

    float halfY = (abb.min.y + abb.max.y) * 0.5f;
    float gap = MapFactory::TILE_SIZE * GAP_FRAC;

    float centerX = ((abb.min.x + abb.max.x) * 0.5f) - gap;
    float centerZ = ((abb.min.z + abb.max.z) * 0.5f) - gap;

    glPushMatrix();
    glTranslatef(centerX, halfY, centerZ);
    glScalef(WALL_THICKNESS_FRAC, 1.0f, WALL_THICKNESS_FRAC);
    glutSolidCube(MapFactory::TILE_SIZE);
    glPopMatrix();
}

void TileWall::renderWallInnerBottomLeft() const {
    BoundingBox3D abb = getAbsoluteBoundingBox();
    glColor3f(0.2f, 0.2f, 0.8f);

    float halfY = (abb.min.y + abb.max.y) * 0.5f;
    float gap = MapFactory::TILE_SIZE * GAP_FRAC;

    float centerX = ((abb.min.x + abb.max.x) * 0.5f) + gap;
    float centerZ = ((abb.min.z + abb.max.z) * 0.5f) + gap;

    glPushMatrix();
    glTranslatef(centerX, halfY, centerZ);
    glScalef(WALL_THICKNESS_FRAC, 1.0f, WALL_THICKNESS_FRAC);
    glutSolidCube(MapFactory::TILE_SIZE);
    glPopMatrix();
}

void TileWall::renderWallInnerBottomRight() const {
    BoundingBox3D abb = getAbsoluteBoundingBox();
    glColor3f(0.2f, 0.2f, 0.8f);

    float halfY = (abb.min.y + abb.max.y) * 0.5f;
    float gap = MapFactory::TILE_SIZE * GAP_FRAC;

    float centerX = ((abb.min.x + abb.max.x) * 0.5f) - gap;
    float centerZ = ((abb.min.z + abb.max.z) * 0.5f) + gap;

    glPushMatrix();
    glTranslatef(centerX, halfY, centerZ);
    glScalef(WALL_THICKNESS_FRAC, 1.0f, WALL_THICKNESS_FRAC);
    glutSolidCube(MapFactory::TILE_SIZE);
    glPopMatrix();
}

void TileWall::setWallTypeStraight() {
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

    bool upGhostHouse = (tu != nullptr) && tu->isGhostHouseTile();
    bool downGhostHouse = (td != nullptr) && td->isGhostHouseTile();
    bool leftGhostHouse = (tl != nullptr) && tl->isGhostHouseTile();
    bool rightGhostHouse = (tr != nullptr) && tr->isGhostHouseTile();

    // Straight walls near the ghost house
    if (openLeft && openRight && !openUp && !openDown && leftGhostHouse) {
        wallType = WallType::LEFT;
        return;
    }
    if (openLeft && openRight && !openUp && !openDown && rightGhostHouse) {
        wallType = WallType::RIGHT;
        return;
    }
    if (openUp && openDown && !openLeft && !openRight && upGhostHouse) {
        wallType = WallType::BOTTOM;
        return;
    }
    if (openUp && openDown && !openLeft && !openRight && downGhostHouse) {
        wallType = WallType::TOP;
        return;
    }

    // Straight walls (corridors on opposite sides -> block)
    if (openLeft && openRight && !openUp && !openDown) {
        // Corridor left and right -> block
        wallType = WallType::BLOCK;
        return;
    }
    if (openUp && openDown && !openLeft && !openRight) {
        // Corridor above and below -> block
        wallType = WallType::BLOCK;
        return;
    }

    // Single-sided segments (corridor on one side -> thin wall on opposite)
    if (openUp && !openDown && !openLeft && !openRight) {
        // Corridor above -> thin top edge
        wallType = WallType::TOP;
        return;
    }
    if (openDown && !openUp && !openLeft && !openRight) {
        // Corridor below -> thin bottom edge
        wallType = WallType::BOTTOM;
        return;
    }
    if (openLeft && !openRight && !openUp && !openDown) {
        // Corridor to left -> thin left edge
        wallType = WallType::RIGHT;
        return;
    }
    if (openRight && !openLeft && !openUp && !openDown) {
        // Corridor to right -> thin right edge
        wallType = WallType::LEFT;
        return;
    }

    // Fallback to full block
    wallType = WallType::BLOCK;
}

void TileWall::setWallTypeOuterCorners() {
    // Only apply to full-block walls
    if (wallType != WallType::BLOCK) {
        return;
    }

    // Get the four cardinal neighbors
    Tile* tu = getTileUp();
    Tile* td = getTileDown();
    Tile* tl = getTileLeft();
    Tile* tr = getTileRight();

    // Get the four diagonal neighbors
    Tile* tul = (tu != nullptr) ? tu->getTileLeft() : nullptr; // up-left
    Tile* tur = (tu != nullptr) ? tu->getTileRight() : nullptr; // up-right
    Tile* tdl = (td != nullptr) ? td->getTileLeft() : nullptr; // down-left
    Tile* tdr = (td != nullptr) ? td->getTileRight() : nullptr; // down-right

    // Otherwise fall back to checking two adjacent walls for L-shaped corners
    bool upWall = (tu != nullptr) && tu->getTileType() == TileType::WALL;
    bool downWall = (td != nullptr) && td->getTileType() == TileType::WALL;
    bool leftWall = (tl != nullptr) && tl->getTileType() == TileType::WALL;
    bool rightWall = (tr != nullptr) && tr->getTileType() == TileType::WALL;
    
    // Determine which diagonal neighbors are walls
    bool ulWall = (tul != nullptr) && (tul->getTileType() == TileType::WALL);
    bool urWall = (tur != nullptr) && (tur->getTileType() == TileType::WALL);
    bool dlWall = (tdl != nullptr) && (tdl->getTileType() == TileType::WALL);
    bool drWall = (tdr != nullptr) && (tdr->getTileType() == TileType::WALL);

    // If all tiles surrounding are walls, keep this block
    if (upWall && downWall && leftWall && rightWall && ulWall && urWall && dlWall && drWall) { return; }

    // If any diagonal neighbor is a corridor (walkable), immediately render this as that outer corner
    if (tul != nullptr && tul->isWalkable()) {
        wallType = WallType::TOP_LEFT_CORNER;
        return;
    }
    if (tur != nullptr && tur->isWalkable()) {
        wallType = WallType::TOP_RIGHT_CORNER;
        return;
    }
    if (tdr != nullptr && tdr->isWalkable()) {
        wallType = WallType::BOTTOM_RIGHT_CORNER;
        return;
    }
    if (tdl != nullptr && tdl->isWalkable()) {
        wallType = WallType::BOTTOM_LEFT_CORNER;
        return;
    }

    // Walls below + right -> top-left corner
    if (downWall && rightWall) {
        wallType = WallType::TOP_LEFT_CORNER;
        return;
    }
    // Walls below + left -> top-right corner
    if (downWall && leftWall) {
        wallType = WallType::TOP_RIGHT_CORNER;
        return;
    }
    // Walls above + right -> bottom-left corner
    if (upWall && rightWall) {
        wallType = WallType::BOTTOM_LEFT_CORNER;
        return;
    }
    // Walls above + left -> bottom-right corner
    if (upWall && leftWall) {
        wallType = WallType::BOTTOM_RIGHT_CORNER;
        return;
    }
}