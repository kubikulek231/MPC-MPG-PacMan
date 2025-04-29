#include "Ghost.h"
#include <GL/glut.h>
#include <random>
#include <queue>
#include <iostream>
#include <unordered_set>

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
    glColor3f(colorR, colorG, colorB);
    glPushMatrix();

    Point3D centerPoint = getAbsoluteCenterPoint();
    // Sphere is defined by center point, so needs to be translated
    glTranslatef(centerPoint.x, centerPoint.y, centerPoint.z);
    glutSolidSphere(0.5f, 16, 16);

    glPopMatrix();

    renderBoundingBox();
    renderOrigin(); 
}

void Ghost::moveOnPath(float frameTimeMs) {
    std::vector<Tile*> tiles = intersectingTiles(this);
    Tile* tile = currentTile(tiles);

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

    int pathSize = movePath.size();
    bool isStandingOnTeleport = tile->getTileType() == TileType::TELEPORT;
    bool isNextInPathTeleport = pathSize > 0 ? movePath.front()->getTileType() == TileType::TELEPORT : false;

    if (isStandingOnTeleport && isNextInPathTeleport && moved) {
        return;
    }

    if (isStandingOnTeleport && isNextInPathTeleport && !moved) {
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
    Tile* bestTile = nullptr;
    float maxDistance = -1.0f;

    auto currentTiles = intersectingTiles(this);
    Tile* startTile = currentTile(currentTiles);
    if (!startTile) return nullptr;

    int startRow = startTile->getTileRow();
    int startCol = startTile->getTileCol();

    int numRows = MapFactory::MAP_HEIGHT;
    int numCols = MapFactory::MAP_WIDTH;

    // Precompute all walkable tiles for the given corner
    std::vector<Tile*> candidateTiles;
    for (int r = 0; r < numRows; ++r) {
        for (int c = 0; c < numCols; ++c) {
            Tile* candidate = map->getTileAt(r, c);
            if (candidate && candidate->isWalkable()) {
                // Filter based on direction to the corner
                bool validTile = false;
                switch (corner) {
                case MapCorner::TOP_LEFT:
                    validTile = (r <= startRow && c <= startCol);
                    break;
                case MapCorner::TOP_RIGHT:
                    validTile = (r <= startRow && c >= startCol);
                    break;
                case MapCorner::BOTTOM_LEFT:
                    validTile = (r >= startRow && c <= startCol);
                    break;
                case MapCorner::BOTTOM_RIGHT:
                    validTile = (r >= startRow && c >= startCol);
                    break;
                }

                if (validTile) {
                    candidateTiles.push_back(candidate);
                }
            }
        }
    }

    // Iterate over filtered candidate tiles to find the furthest tile
    for (Tile* candidate : candidateTiles) {
        float distance = heuristicCost(startTile, candidate);
        if (distance > maxDistance) {
            // If this candidate tile is further, consider it as the best
            maxDistance = distance;
            bestTile = candidate;
        }
    }

    return bestTile;
}
