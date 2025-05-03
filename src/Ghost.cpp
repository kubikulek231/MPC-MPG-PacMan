#include "Ghost.h"
#include <GL/glut.h>
#include <random>
#include <queue>
#include <iostream>
#include <unordered_set>
#include "RenderHelper.h"
#include "GameLighting.h"
#include "Pi.h"

Ghost::Ghost() {
}

Ghost::Ghost(const Ghost& other) {
    this->boundingBox = other.boundingBox;
    this->origin = other.origin;
}

Ghost::Ghost(Map* map, Point3D ghostOrigin, BoundingBox3D ghostBoundingBox, std::string name) 
: MovableEntity(map,
                ghostOrigin,
                ghostBoundingBox,
                Ghost::DEFAULT_MOVE_DIR,
                Ghost::DEFAULT_SPEED,
                Ghost::DEFAULT_SNAP_DISTANCE,
                Ghost::DEFAULT_DIR_CHANGE_REQUEST_EXPIRE,
                Ghost::DEFAULT_DIR_CHANGE_REQUEST_EXPIRE_AFTER_MS) {
    this->name = name;
}

void Ghost::render() {
    GLfloat matAmbient[] = { colorR * 0.2f,  colorG * 0.2f,  colorB * 0.2f, 1.0f };
    GLfloat matDiffuse[] = { colorR * 0.5f, colorG * 0.5f, colorB * 0.5f, 1.0f };
    GLfloat matSpecular[] = { 0.05f, 0.05f, 0.05f, 1.0f };
    GLfloat matEmission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat matShininess = 64.0f;

    GameLighting::setMaterial( GL_FRONT_AND_BACK, matAmbient, matDiffuse, matSpecular, matEmission, matShininess);

    glPushMatrix();
    Point3D centerPoint = getAbsoluteCenterPoint();

    // Translate before rotation
    glTranslatef(centerPoint.x, centerPoint.y + 0.25, centerPoint.z);

    glutSolidSphere(0.75f, 18, 18);

    // Skirt
    glPushMatrix();
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        glRotatef(180.0f, 0.0f, 1.0f, 0.0f);

        const float radius = 0.75f;
        const float height = 0.45f;
        const int   segments = 128;
        const int   waves = 12;
        const float waveAmplitude = 0.3f;

        // First half: baseline -> trough
        const float B1_P0 = 0.0f;                       // start at baseline
        const float B1_P1 = -0.2f * waveAmplitude;      // first handle (entry slope)
        const float B1_P2 = -0.9f * waveAmplitude;      // second handle (approach trough)
        const float B1_P3 = -waveAmplitude;             // reach trough

        // Second half: trough -> baseline
        const float B2_P0 = -waveAmplitude;             // start at trough
        const float B2_P1 = -0.9f * waveAmplitude;      // first handle (leaving trough)
        const float B2_P2 = -0.2f * waveAmplitude;      // second handle (approach baseline)
        const float B2_P3 = 0.0f;                       // return to baseline


        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i <= segments; ++i) {
            float theta = 2.0f * PI * i / segments;
            float x = radius * cosf(theta);
            float y = radius * sinf(theta);

            float phase = waves * theta / (2.0f * PI);
            int   windex = int(floorf(phase));
            float localT = phase - windex;

            // split into two Bezier segments
            float wave;
            if (localT < 0.5f) {
                float t1 = localT * 2.0f;
                wave = RenderHelper::cubicBezier(
                    B1_P0, B1_P1, B1_P2, B1_P3,
                    t1
                );
            }
            else {
                float t2 = (localT - 0.5f) * 2.0f;
                wave = RenderHelper::cubicBezier(
                    B2_P0, B2_P1, B2_P2, B2_P3,
                    t2
                );
            }

            // Normals & vertices
            float nx = cosf(theta), ny = sinf(theta);
            glNormal3f(nx, ny, 0.0f);
            glVertex3f(x, y, -height + wave);
            glVertex3f(x, y, 0.0f);
        }
        glEnd();
    glPopMatrix();

    GameLighting::resetMaterial(GL_FRONT_AND_BACK);

    glPushMatrix();
        // Rotate Ghost to face movement direction
        float moveDirAngle = getMoveDirRotationAngle();
        glRotatef(moveDirAngle, 0.0f, 1.0f, 0.0f);

        // Align Ghost vertically (facing direction)
        glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
        // --- EYES & PUPILS ---
        // Eye
        GLfloat eyeAmbient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
        GLfloat eyeDiffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };  // White for the eyes
        GLfloat eyeSpecular[4] = { 0.9f, 0.9f, 0.9f, 1.0f }; // Shiny eyes
        GLfloat eyeEmission[4] = { 0.0f, 0.0f, 0.0f, 1.0f };  // No emission
        GLfloat eyeShininess = 128.0f;

        auto drawEye = [](float tx, float ty, float tz, float rz, float ry) {
            glPushMatrix();
                glColor3f(1.0f, 1.0f, 1.0f);
                glTranslatef(tx, ty, tz);
                glRotatef(rz, 0.0f, 0.0f, 1.0f);
                glRotatef(ry, 0.0f, 1.0f, 0.0f);
                glScalef(1.0f, 1.0f, 0.3f);
                glutSolidSphere(0.20f, 12, 12);
            glPopMatrix();
            };

        // Pupil
        GLfloat pupilAmbient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };  // Black ambient
        GLfloat pupilDiffuse[4] = { 0.0f, 0.0f, 0.0f, 1.0f };  // Black diffuse
        GLfloat pupilSpecular[4] = { 0.2f, 0.2f, 0.2f, 1.0f }; // Slightly shiny (small specular)
        GLfloat pupilEmission[4] = { 0.0f, 0.0f, 0.0f, 1.0f };  // No emission
        GLfloat pupilShininess = 10.0f;  // Low shininess

        auto drawPupil = [](float tx, float ty, float tz, float rz, float ry) {
            glPushMatrix();
            glTranslatef(tx, ty, tz);
            glRotatef(rz, 0.0f, 0.0f, 1.0f);
            glRotatef(ry, 0.0f, 1.0f, 0.0f);
            glScalef(1.0f, 1.0f, 0.3f);
            glutSolidSphere(0.11f, 12, 12);
            glPopMatrix();
            };

        GameLighting::setMaterial(GL_FRONT_AND_BACK, eyeAmbient, eyeDiffuse, eyeSpecular, eyeEmission, eyeShininess);
        drawEye(0.54f, 0.27f, -0.41f, 35.0f, -55.0f); // Left eye
        drawEye(0.54f, -0.27f, -0.41f, 145.0f, -125.0f); // Right eye
        GameLighting::resetMaterial(GL_FRONT_AND_BACK);

        GameLighting::setMaterial(GL_FRONT_AND_BACK, pupilAmbient, pupilDiffuse, pupilSpecular, pupilEmission, pupilShininess);
        drawPupil(0.535f, 0.27f, -0.50f, 40.0f, -44.5f); // Left pupil
        drawPupil(0.535f, -0.27f, -0.50f, 140.0f, -135.5f); // Right pupil
        GameLighting::resetMaterial(GL_FRONT_AND_BACK);
    glPopMatrix();
    glPopMatrix();
    //renderBoundingBox();
    //renderOrigin(); 
}

void Ghost::moveOnPath(float frameTimeMs) {
    std::vector<Tile*> tiles = intersectingTiles(this);
    Tile* tile = currentTile(tiles);

    if (this->name == "blinky") {
        std::cout << "";
    }

    if (!tile || movePath.empty()) {
        moveDir = MoveDir::NONE;
        return;
    }

    // Do nothing when path is empty
    if (moveDir == MoveDir::UNDEFINED || moveDir == MoveDir::NONE) {
        if (movePath.empty()) {
            moveDir = MoveDir::NONE;
            return;
        }
    }

    Tile* nextTile = movePath.front();
    // Get next moveDir
    if (nextTile->isNeighbor(tile)) {
        // If the current tile and next tile differ, get the moveDir
        moveDir = dirToTile(tile, nextTile);
    }

    bool moved = false;
    bool inCenter = false;
    this->preciseMoveToNextTile(moveDir, frameTimeMs, moved, inCenter, tiles);

    // Check for pending tile path change
    // Only generate new path and switch to it if in center
    if (tileToSwitchPathTo != nullptr && inCenter) {
        movePath.clear();
        createPathToTile(tileToSwitchPathTo);
        tileToSwitchPathTo = nullptr;
    }

    int pathSize = movePath.size();
    bool isStandingOnTeleport = tile->getTileType() == TileType::TELEPORT;
    bool isCurrentInPathTeleport = pathSize > 0 ? movePath.front()->getTileType() == TileType::TELEPORT : false;
    bool isNexttInPathTeleport = pathSize > 1 ? movePath.at(1)->getTileType() == TileType::TELEPORT : false;
    bool nextTileToBeHitIsTeleport = nextTileInDirection(moveDir, tile)->getTileType() == TileType::TELEPORT;

    // Just move if we want to teleport but are not quite in the spot yet
    if (isCurrentInPathTeleport && isNexttInPathTeleport && moved) {
        return;
    }

    if (isStandingOnTeleport && isCurrentInPathTeleport && !moved) {
        teleport(moveDir);
        movePath.pop_front();
        return;
    }

    if (inCenter && movePath.front()->isEqual(tile)) {
        movePath.pop_front();
        return;
    }
}

void Ghost::createPathToTile(Tile* tile) {
    this->movePath = shortestPathToTile(tile);
    auto tiles = this->intersectingTiles(this);
    auto entityTile = currentTile(tiles);
    if (movePath.size() > 0 && movePath.front() == entityTile) { movePath.pop_front(); }
}

void Ghost::createAndSetPathToTileWhenPossible(Tile* tile) {
    tileToSwitchPathTo = tile;
}

void Ghost::randomMove(float frameTimeMs) {
    bool change = true;
    // Set initial direction
    if (moveDir == MoveDir::NONE || moveDir == MoveDir::UNDEFINED) {
        moveDir = randomDirection();
    }
    change = true;
    while (!this->move(moveDir, change, frameTimeMs)) {
        moveDir = randomTurnDirection();
    }
}

void Ghost::moveOnRandomPath(float frameTimeMs) {
    if (moveDir == MoveDir::NONE || moveDir == MoveDir::UNDEFINED) {
        moveDir = randomDirection();
    }

    if (!movePath.empty()) {
        moveOnPath(frameTimeMs);
    }

    // Pick new path
    while (movePath.empty()) {
        Tile* tile = map->getRandomTile();
        this->createPathToTile(tile);
    }
}

MoveDir Ghost::randomDirection() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 3); // assuming 4 directions

    int random = dist(gen);
    switch (random) {
    case 0: return MoveDir::FWD;
    case 1: return MoveDir::BWD;
    case 2: return MoveDir::LEFT;
    case 3: return MoveDir::RIGHT;
    }
}

MoveDir Ghost::randomTurnDirection() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 1); // binary choice

    int random = dist(gen);
    switch (moveDir) {
    case MoveDir::FWD:
    case MoveDir::BWD:
        return (random == 0) ? MoveDir::LEFT : MoveDir::RIGHT;
    case MoveDir::LEFT:
    case MoveDir::RIGHT:
        return (random == 0) ? MoveDir::FWD : MoveDir::BWD;
    }
}

bool Ghost::randomBool() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    bool outBool;
    std::uniform_int_distribution<int> boolDist(0, 1);     // true or false
    outBool = boolDist(gen); // sets the reference param to true or false
    return outBool;
}

bool Ghost::randomBoolWithChance(float chance) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return dist(gen) < chance;
}

std::deque<Tile*> Ghost::shortestPathToTile(Tile* targetTile) {
    if (!targetTile || !targetTile->isWalkable()) return {};

    auto currentTiles = intersectingTiles(this);
    Tile* startTile = this->currentTile(currentTiles);
    if (!startTile) return {};

    std::unordered_map<Tile*, Tile*> cameFrom;
    std::unordered_map<Tile*, float> gScore;
    std::unordered_map<Tile*, float> fScore;

    auto cmp = [&](Tile* a, Tile* b) {
        return fScore[a] > fScore[b];
        };

    std::priority_queue<Tile*, std::vector<Tile*>, decltype(cmp)> openSet(cmp);
    std::unordered_set<Tile*> openSetLookup;

    gScore[startTile] = 0.0f;
    fScore[startTile] = heuristicCost(startTile, targetTile);
    openSet.push(startTile);
    openSetLookup.insert(startTile);

    while (!openSet.empty()) {
        Tile* current = openSet.top();
        openSet.pop();
        openSetLookup.erase(current);

        if (current->getTileRow() == targetTile->getTileRow() &&
            current->getTileCol() == targetTile->getTileCol()) {
            return reconstructPath(cameFrom, current);
        }

        for (Tile* neighbor : getNeighbors(current)) {

            float tentativeG = gScore[current] + distance(current, neighbor);
            if (!gScore.count(neighbor) || tentativeG < gScore[neighbor]) {
                cameFrom[neighbor] = current;
                gScore[neighbor] = tentativeG;
                fScore[neighbor] = tentativeG + heuristicCost(neighbor, targetTile);

                if (!openSetLookup.count(neighbor)) {
                    openSet.push(neighbor);
                    openSetLookup.insert(neighbor);
                }
            }
        }
    }

    return {}; // No path found
}

float Ghost::distance(Tile* a, Tile* b) {
    return 1.0f; // constant for grid-based movement
}

float Ghost::heuristicCost(Tile* a, Tile* b) {
    return std::abs(a->getTileRow() - b->getTileRow()) + std::abs(a->getTileCol() - b->getTileCol()); // Manhattan Distance
}

std::vector<Tile*> Ghost::getNeighbors(Tile* tile) {
    std::vector<Tile*> neighbors;
    Tile* up = tile->getTileUp();
    if (up && up->isWalkable()) neighbors.push_back(up);
    Tile* down = tile->getTileDown();
    if (down && down->isWalkable()) neighbors.push_back(down);
    Tile* left = tile->getTileLeft();
    if (left && left->isWalkable()) neighbors.push_back(left);
    Tile* right = tile->getTileRight();
    if (right && right->isWalkable()) neighbors.push_back(right);
    return neighbors;
}

std::deque<Tile*> Ghost::reconstructPath(std::unordered_map<Tile*, Tile*> cameFrom, Tile* current)
{
    std::deque<Tile*> path;
    while (cameFrom.find(current) != cameFrom.end()) {
        path.push_back(current);
        current = cameFrom[current];
    }
    std::reverse(path.begin(), path.end());
    return path;
}

Tile* Ghost::furthestTileTowardCorner(MapCorner corner) {
    auto currentTiles = intersectingTiles(this);
    Tile* startTile = currentTile(currentTiles);
    if (!startTile) return nullptr;

    int startRow = startTile->getTileRow();
    int startCol = startTile->getTileCol();

    int numRows = MapFactory::MAP_HEIGHT;
    int numCols = MapFactory::MAP_WIDTH;

    // Define corner bounds
    int rowStart = (corner == MapCorner::TOP_LEFT || corner == MapCorner::TOP_RIGHT) ? 0 : startRow;
    int rowEnd = (corner == MapCorner::TOP_LEFT || corner == MapCorner::TOP_RIGHT) ? startRow : numRows;

    int colStart = (corner == MapCorner::TOP_LEFT || corner == MapCorner::BOTTOM_LEFT) ? 0 : startCol;
    int colEnd = (corner == MapCorner::TOP_LEFT || corner == MapCorner::BOTTOM_LEFT) ? startCol : numCols;

    struct Candidate {
        Tile* tile;
        float distance;
    };
    std::vector<Candidate> candidates;

    // Gather all valid walkable tiles within the direction range
    for (int r = rowStart; r < rowEnd; ++r) {
        for (int c = colStart; c < colEnd; ++c) {
            Tile* tile = map->getTileAt(r, c);
            if (!tile || !tile->isWalkable()) continue;

            float distance = heuristicCost(startTile, tile);
            candidates.push_back({ tile, distance });
        }
    }

    // Sort by descending distance
    std::sort(candidates.begin(), candidates.end(), [](const Candidate& a, const Candidate& b) {
        return a.distance > b.distance;
        });

    // Return the first reachable candidate
    for (const Candidate& c : candidates) {
        if (!shortestPathToTile(c.tile).empty()) {
            return c.tile;
        }
    }

    return nullptr;
}