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

Ghost::Ghost(Map* map, Point3D ghostOrigin, BoundingBox3D ghostBoundingBox) 
: MovableEntity(map,
                ghostOrigin,
                ghostBoundingBox,
                Ghost::DEFAULT_MOVE_DIR,
                Ghost::DEFAULT_SPEED,
                Ghost::DEFAULT_SNAP_DISTANCE,
                Ghost::DEFAULT_DIR_CHANGE_REQUEST_EXPIRE,
                Ghost::DEFAULT_DIR_CHANGE_REQUEST_EXPIRE_AFTER_MS) {
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

void Ghost::moveToTile(float frameTimeMs, Tile* tile) {
    std::cout << "current path to tile:" << std::endl;
    if (!tile || !tile->isWalkable()) { return; }
    for (auto tile : shortestPathToTile(tile)) {
        std::cout << tile->toString() << std::endl;
    }
}

void Ghost::moveOnPath(float frameTimeMs) {
    bool moved = false;
    bool inCenter = false;

    std::cout << "current path:" << std::endl;
    for (auto tile : movePath) {
        std::cout << tile->toString() << std::endl;
    }

    std::vector<Tile*> tiles = intersectingTiles(this);
    Tile* tile = currentTile(tiles);

    if (!tile || movePath.empty()) {
        moveDir = MoveDir::NONE;
        return;
    }

    if (moveDir == MoveDir::UNDEFINED) {
        Tile* nextTile = movePath.front();
        if (!nextTile) return;
        moveDir = dirToTile(tile, nextTile);
    }

    if (moveDir == MoveDir::NONE || movePath.empty()) {
        return;
    }

    // Use our new center-snapping method
    preciseMoveToNextTile(moveDir, frameTimeMs, moved, inCenter, tiles);

    // If we reached the center of the next tile and have more path to follow
    if (inCenter && !movePath.empty()) {
        Tile* oldTile = movePath.front();
        movePath.pop_front();

        if (!movePath.empty()) {
            Tile* nextTile = movePath.front();
            moveDir = dirToTile(oldTile, nextTile);
        }
        else {
            moveDir = MoveDir::NONE;
        }
    }
}


void Ghost::createPathToTile(Tile* tile) {
    this->movePath = shortestPathToTile(tile);
}

void Ghost::randomMove(float frameTimeMs) {
    bool moved = false;
    bool turning = false;
    bool canTurn = false;
    std::vector<Tile*> tiles = intersectingTiles(this);
    while (true) {
        if (moveDir == MoveDir::NONE) {
            moveDir = randomDirection();
            continue;
        }
        if (randomBoolWithChance(0.1)) {
            turning = true;
        }

        if (turning) {
            this->preciseMoveUntilCanTurn(moveDir, frameTimeMs, canTurn, moved, tiles);
            if (canTurn) {
                // Choose a direction to turn
                moveDir = randomDirection();
                turning = false;
                return;
            }
            return;
        }

        this->preciseMove(moveDir, frameTimeMs, moved);
        if (moved) { break; }
        moveDir = randomDirection();
        if (randomBoolWithChance(0.05)) {
            turning = true;
        }
    }
    return;
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
