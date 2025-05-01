#include "TileWall.h"
#include "MapFactory.h"

#define PI 3.14159265358979323846f

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
    renderEmpty();
    switch (wallType) {
    case WallType::BLOCK:
        renderWallBlock();
        break;
    case WallType::RIGHT:
        renderWallLeft();
        break;
    case WallType::LEFT:
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
    glColor3f(WALL_COLOR[0], WALL_COLOR[1], WALL_COLOR[2]); // Blue for wall

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
    glColor3f(WALL_COLOR[0], WALL_COLOR[1], WALL_COLOR[2]);

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
    glColor3f(WALL_COLOR[0], WALL_COLOR[1], WALL_COLOR[2]);

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
    glColor3f(WALL_COLOR[0], WALL_COLOR[1], WALL_COLOR[2]);

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
    glColor3f(WALL_COLOR[0], WALL_COLOR[1], WALL_COLOR[2]);

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

#include <GL/glu.h>

// file-scope callbacks and state
static double g_tessY;
static void CALLBACK tessBeginCallback(GLenum type) { glBegin(type); }
static void CALLBACK tessEndCallback() { glEnd(); }
static void CALLBACK tessVertexCallback(void* data) {
    const GLdouble* xy = (const GLdouble*)data;
    glVertex3d(xy[0], g_tessY, xy[1]);
}

void TileWall::drawRectMinusQuarter(float r, float height,
    float startAngle, float endAngle,
    int segs) const
{
    float halfH = height * 0.5f;
    float delta = (endAngle - startAngle) / segs;

    // 1) Build the wall contour pts
    std::vector<std::pair<float, float>> pts;
    pts.emplace_back(0.0f, r);
    pts.emplace_back(-r, r);
    pts.emplace_back(-r, 0.0f);
    for (int i = 0; i <= segs; ++i) {
        float a = endAngle - i * delta;
        pts.emplace_back(cosf(a) * r, sinf(a) * r);
    }

    // 2) Draw the side walls (unchanged)
    glBegin(GL_QUAD_STRIP);
    for (auto& p : pts) {
        glVertex3f(p.first, +halfH, p.second);
        glVertex3f(p.first, -halfH, p.second);
    }
    // close
    glVertex3f(pts[0].first, +halfH, pts[0].second);
    glVertex3f(pts[0].first, -halfH, pts[0].second);
    glEnd();

    // 3) Copy pts into a flat GLdouble array for tessellation
    std::vector<GLdouble> contour;
    contour.reserve(pts.size() * 2);
    for (auto& p : pts) {
        contour.push_back(p.first);
        contour.push_back(p.second);
    }

    // 4) Create & configure the tessellator
    GLUtesselator* tess = gluNewTess();
    gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_NONZERO);
    gluTessCallback(tess, GLU_TESS_BEGIN, (void(*)())tessBeginCallback);
    gluTessCallback(tess, GLU_TESS_END, (void(*)())tessEndCallback);
    gluTessCallback(tess, GLU_TESS_VERTEX, (void(*)())tessVertexCallback);

    // 5) Top cap (+halfH)
    g_tessY = +halfH;
    gluTessBeginPolygon(tess, nullptr);
    gluTessBeginContour(tess);
    for (size_t i = 0; i < contour.size() / 2; ++i) {
        gluTessVertex(tess,
            &contour[i * 2],
            &contour[i * 2]);
    }
    gluTessEndContour(tess);
    gluTessEndPolygon(tess);

    // 6) Bottom cap (–halfH), reverse winding
    g_tessY = -halfH;
    gluTessBeginPolygon(tess, nullptr);
    gluTessBeginContour(tess);
    for (int i = (int)contour.size() / 2 - 1; i >= 0; --i) {
        gluTessVertex(tess,
            &contour[i * 2],
            &contour[i * 2]);
    }
    gluTessEndContour(tess);
    gluTessEndPolygon(tess);

    gluDeleteTess(tess);
}


void TileWall::renderWallCornerTopLeft() const {
    renderWallTop();
    renderWallRight();

    auto bb = getAbsoluteBoundingBox();
    float halfY = (bb.min.y + bb.max.y) * 0.5f;
    float tileH = bb.max.y - bb.min.y;
    float r = INNER_RADIUS_FRAC;
    float w = WALL_THICKNESS_FRAC;

    // position corner
    float cx = bb.min.x + (w + r);
    float cz = bb.max.z - (w + r);

    glDisable(GL_CULL_FACE);
    glColor3f(1, 0, 0);

    glPushMatrix();
    glTranslatef(cx, halfY + 1.0f, cz);

    drawRectMinusQuarter(r, tileH, PI * 0.5f, PI, CYLINDER_SEGMENTS);
    glPopMatrix();
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

// helper: draw an axis-aligned box [x0,x1]×[y0,y1]×[z0,z1]
void TileWall::drawBox(float x0, float x1,
    float y0, float y1,
    float z0, float z1) const
{
    glBegin(GL_QUADS);
    // front (z=z1)
    glNormal3f(0, 0, 1);
    glVertex3f(x0, y0, z1);
    glVertex3f(x1, y0, z1);
    glVertex3f(x1, y1, z1);
    glVertex3f(x0, y1, z1);

    // back (z=z0)
    glNormal3f(0, 0, -1);
    glVertex3f(x1, y0, z0);
    glVertex3f(x0, y0, z0);
    glVertex3f(x0, y1, z0);
    glVertex3f(x1, y1, z0);

    // left (x=x0)
    glNormal3f(-1, 0, 0);
    glVertex3f(x0, y0, z0);
    glVertex3f(x0, y0, z1);
    glVertex3f(x0, y1, z1);
    glVertex3f(x0, y1, z0);

    // right (x=x1)
    glNormal3f(1, 0, 0);
    glVertex3f(x1, y0, z1);
    glVertex3f(x1, y0, z0);
    glVertex3f(x1, y1, z0);
    glVertex3f(x1, y1, z1);

    // top (y=y1)
    glNormal3f(0, 1, 0);
    glVertex3f(x0, y1, z1);
    glVertex3f(x1, y1, z1);
    glVertex3f(x1, y1, z0);
    glVertex3f(x0, y1, z0);

    // bottom (y=y0)
    glNormal3f(0, -1, 0);
    glVertex3f(x0, y0, z0);
    glVertex3f(x1, y0, z0);
    glVertex3f(x1, y0, z1);
    glVertex3f(x0, y0, z1);
    glEnd();
}


void TileWall::drawQuarterCylinder(float radius, float height, float startAngle, float endAngle, int segs) const {
    float halfH = height * 0.5f;
    float step = (endAngle - startAngle) / segs;

    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= segs; ++i) {
        float a = startAngle + i * step;
        float x = cosf(a) * radius;
        float z = sinf(a) * radius;
        // bottom vertex
        glVertex3f(x, -halfH, z);
        // top vertex
        glVertex3f(x, halfH, z);
    }
    glEnd();
}

void TileWall::drawSolidQuarterCylinder(float radius, float height, float angleStart, float angleEnd, int segments) const {
    float delta = (angleEnd - angleStart) / segments;

    // Curved outer wall
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segments; ++i) {
        float angle = angleStart + i * delta;
        float x = cosf(angle) * radius;
        float z = sinf(angle) * radius;
        glVertex3f(x, -height * 0.5f, z);
        glVertex3f(x, height * 0.5f, z);
    }
    glEnd();

    // Bottom cap
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, -height * 0.5f, 0);  // center
    for (int i = 0; i <= segments; ++i) {
        float angle = angleStart + i * delta;
        float x = cosf(angle) * radius;
        float z = sinf(angle) * radius;
        glVertex3f(x, -height * 0.5f, z);
    }
    glEnd();

    // Top cap
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, height * 0.5f, 0);  // center
    for (int i = 0; i <= segments; ++i) {
        float angle = angleStart + i * delta;
        float x = cosf(angle) * radius;
        float z = sinf(angle) * radius;
        glVertex3f(x, height * 0.5f, z);
    }
    glEnd();

    // Side wall 1 (start angle)
    {
        float x = cosf(angleStart) * radius;
        float z = sinf(angleStart) * radius;
        glBegin(GL_QUADS);
        glVertex3f(0, -height * 0.5f, 0);
        glVertex3f(0, height * 0.5f, 0);
        glVertex3f(x, height * 0.5f, z);
        glVertex3f(x, -height * 0.5f, z);
        glEnd();
    }

    // Side wall 2 (end angle)
    {
        float x = cosf(angleEnd) * radius;
        float z = sinf(angleEnd) * radius;
        glBegin(GL_QUADS);
        glVertex3f(0, -height * 0.5f, 0);
        glVertex3f(0, height * 0.5f, 0);
        glVertex3f(x, height * 0.5f, z);
        glVertex3f(x, -height * 0.5f, z);
        glEnd();
    }
}

void TileWall::drawConcaveQuarterCylinder(float radius, float height,
    float startAngle, float endAngle,
    int segs) const
{
    const float halfH = height * 0.5f;
    const float step = (endAngle - startAngle) / segs;

    // --- 1) Inner curved surface (normals pointing _inward_) ---
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segs; ++i) {
        float a = startAngle + i * step;
        float x = cosf(a) * radius;
        float z = sinf(a) * radius;
        // point inward
        glNormal3f(-cosf(a), 0.0f, -sinf(a));
        // top then bottom
        glVertex3f(x, halfH, z);
        glVertex3f(x, -halfH, z);
    }
    glEnd();

    // --- 2) Top “floor” of the carve (flat fan from corner to arc) ---
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, +1, 0);
    glVertex3f(0, halfH, 0);  // fan center
    for (int i = 0; i <= segs; ++i) {
        float a = startAngle + i * step;
        glVertex3f(cosf(a) * radius, halfH, sinf(a) * radius);
    }
    glEnd();

    // --- 3) Bottom “ceiling” of the carve (flat fan from corner to arc) ---
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, -1, 0);
    glVertex3f(0, -halfH, 0);  // fan center
    for (int i = 0; i <= segs; ++i) {
        float a = startAngle + i * step;
        glVertex3f(cosf(a) * radius, -halfH, sinf(a) * radius);
    }
    glEnd();

    // --- 4) Side wall at startAngle ---
    {
        float x0 = cosf(startAngle) * radius;
        float z0 = sinf(startAngle) * radius;
        glBegin(GL_QUADS);
        // normal points into the carved volume
        glNormal3f(sinf(startAngle), 0, -cosf(startAngle));
        glVertex3f(x0, halfH, z0);
        glVertex3f(x0, -halfH, z0);
        glVertex3f(0.0f, -halfH, 0.0f);
        glVertex3f(0.0f, halfH, 0.0f);
        glEnd();
    }

    // --- 5) Side wall at endAngle ---
    {
        float x1 = cosf(endAngle) * radius;
        float z1 = sinf(endAngle) * radius;
        glBegin(GL_QUADS);
        glNormal3f(-sinf(endAngle), 0, +cosf(endAngle));
        glVertex3f(x1, halfH, z1);
        glVertex3f(x1, -halfH, z1);
        glVertex3f(0.0f, -halfH, 0.0f);
        glVertex3f(0.0f, halfH, 0.0f);
        glEnd();
    }
}



// ------ inner-corner renders ------
void TileWall::renderWallInnerTopLeft() const {
    auto bb = getAbsoluteBoundingBox();
    float halfY = (bb.min.y + bb.max.y) * 0.5f;
    float tileH = bb.max.y - bb.min.y;
    float r = INNER_RADIUS_FRAC;
    float wall = WALL_THICKNESS_FRAC;

    // place cylinder exactly in tile corner
    float cx = bb.min.x + (wall + r);
    float cz = bb.max.z - (wall + r);

    glDisable(GL_CULL_FACE);
    glColor3f(WALL_COLOR[0], WALL_COLOR[1], WALL_COLOR[2]);

    glPushMatrix();
    glTranslatef(cx, halfY, cz);

    // Quarter-cylinder in the corner
    drawSolidQuarterCylinder(r, tileH, PI * 0.5f, PI, CYLINDER_SEGMENTS);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(bb.min.x, 0.0f, bb.min.z);
    // Brick strip along X (horizontal bar)
    drawBox(MapFactory::TILE_SIZE, wall, 0.0f, MapFactory::TILE_SIZE, (wall - r), 0.0f);
    // Brick strip along Z (vertical bar)
    drawBox(MapFactory::TILE_SIZE, wall + r, 0.0f, MapFactory::TILE_SIZE, wall, 0.0f);
    glPopMatrix();
}

void TileWall::renderWallInnerTopRight() const {
    auto bb = getAbsoluteBoundingBox();
    float halfY = (bb.min.y + bb.max.y) * 0.5f;
    float tileH = bb.max.y - bb.min.y;
    float r = INNER_RADIUS_FRAC;
    float wall = WALL_THICKNESS_FRAC;

    float cx = bb.max.x - (wall + r);
    float cz = bb.max.z - (wall + r);

    glDisable(GL_CULL_FACE);
    glColor3f(WALL_COLOR[0], WALL_COLOR[1], WALL_COLOR[2]);

    glPushMatrix();
    glTranslatef(cx, halfY, cz);
    drawSolidQuarterCylinder(r, tileH, 0.0f, PI * 0.5f, CYLINDER_SEGMENTS);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(bb.max.x, 0.0f, bb.min.z);
    // Horizontal strip
    drawBox(-MapFactory::TILE_SIZE, -wall, 0.0f, MapFactory::TILE_SIZE, (wall - r), 0.0f);
    // Vertical strip
    drawBox(-MapFactory::TILE_SIZE, -(wall + r), 0.0f, MapFactory::TILE_SIZE, wall, 0.0f);
    glPopMatrix();
}

void TileWall::renderWallInnerBottomLeft() const {
    auto bb = getAbsoluteBoundingBox();
    float halfY = (bb.min.y + bb.max.y) * 0.5f;
    float tileH = bb.max.y - bb.min.y;
    float r = INNER_RADIUS_FRAC;
    float wall = WALL_THICKNESS_FRAC;

    float cx = bb.min.x + (wall + r);
    float cz = bb.min.z + (wall + r);

    glDisable(GL_CULL_FACE);
    glColor3f(WALL_COLOR[0], WALL_COLOR[1], WALL_COLOR[2]);

    glPushMatrix();
    glTranslatef(cx, halfY, cz);
    drawSolidQuarterCylinder(r, tileH, PI, PI * 1.5f, CYLINDER_SEGMENTS);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(bb.min.x, 0.0f, bb.max.z);
    // Horizontal strip
    drawBox(MapFactory::TILE_SIZE, wall, 0.0f, MapFactory::TILE_SIZE, -wall + r, 0.0f);
    // Vertical strip
    drawBox(MapFactory::TILE_SIZE, wall + r, 0.0f, MapFactory::TILE_SIZE, -wall, 0.0f);
    glPopMatrix();
}

void TileWall::renderWallInnerBottomRight() const {
    auto bb = getAbsoluteBoundingBox();
    float halfY = (bb.min.y + bb.max.y) * 0.5f;
    float tileH = bb.max.y - bb.min.y;
    float r = INNER_RADIUS_FRAC;
    float wall = WALL_THICKNESS_FRAC;

    float cx = bb.max.x - (wall + r);
    float cz = bb.min.z + (wall + r);

    glDisable(GL_CULL_FACE);
    glColor3f(WALL_COLOR[0], WALL_COLOR[1], WALL_COLOR[2]);

    glPushMatrix();
    glTranslatef(cx, halfY, cz);
    drawSolidQuarterCylinder(r, tileH, PI * 1.5f, 2.0f * PI, CYLINDER_SEGMENTS);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(bb.max.x, 0.0f, bb.max.z);
    // Horizontal strip
    drawBox(-MapFactory::TILE_SIZE, -wall, 0.0f, MapFactory::TILE_SIZE, -wall + r, 0.0f);
    // Vertical strip
    drawBox(-MapFactory::TILE_SIZE, -(wall + r), 0.0f, MapFactory::TILE_SIZE, -wall, 0.0f);
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
        wallType = WallType::RIGHT;
        return;
    }
    if (openLeft && openRight && !openUp && !openDown && rightGhostHouse) {
        wallType = WallType::LEFT;
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
    
    // Straight walls near doors
    if (openUp && openDown && openRight && !openLeft && rightGhostHouse) {
        wallType = WallType::TOP;
        return;
    }
    if (openUp && openDown && openLeft && !openRight && leftGhostHouse) {
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
        wallType = WallType::LEFT;
        return;
    }
    if (openRight && !openLeft && !openUp && !openDown) {
        // Corridor to right -> thin right edge
        wallType = WallType::RIGHT;
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