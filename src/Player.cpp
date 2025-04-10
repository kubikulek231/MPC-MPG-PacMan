#include "Player.h"
#include <GL/glut.h>

Player::Player() {
}

Player::Player(const Player& other) {
    this->boundingBox = other.boundingBox;
    this->origin = other.origin;
}

Player::Player(Map* map, Point3D playerOrigin, BoundingBox3D playerBoundingBox) 
: MovableEntity(map,
                playerOrigin,
                playerBoundingBox,
                Player::DEFAULT_MOVE_DIR,
                Player::DEFAULT_SPEED,
                Player::DEFAULT_SNAP_DISTANCE,
                Player::DEFAULT_DIR_CHANGE_REQUEST_EXPIRE,
                Player::DEFAULT_DIR_CHANGE_REQUEST_EXPIRE_AFTER_MS) {
}

void Player::render() {
    glColor3f(1.0f, 1.0f, 0.0f);
    glPushMatrix();

    Point3D centerPoint = getAbsoluteCenterPoint();
    // Sphere is defined by center point, so needs to be translated
    glTranslatef(centerPoint.x, centerPoint.y, centerPoint.z);
    glutSolidSphere(0.5f, 16, 16);

    glPopMatrix();

    renderBoundingBox();
    renderOrigin();
}
//
//Player::move() {
//
//    void MovableEntity::move() {
//        MoveDir userMoveDir = *game.getMoveDir();
//
//        if (userMoveDir == MoveDir::NONE) { return; }
//
//        bool userChangedDir = game.getIsDirectionKeyPressed();
//        game.setIsDirectionKeyPressed(false);
//
//        // User pressed a dir key
//        if (userChangedDir) {
//            // Delete existing request
//            delete dirChangeRequest;
//            dirChangeRequest = nullptr;
//            // If actual direction and requested direction differ, set new change request
//            if (userMoveDir != actualMoveDir) {
//                dirChangeRequest = new DirChangeRequest(userMoveDir);
//            }
//        }
//
//        // If the same direction as before, just keep going
//        if (userMoveDir == actualMoveDir) {
//            delete dirChangeRequest;
//            dirChangeRequest = nullptr;
//            GameLogic::movePlayer(game.getFrametimeNormalizedSpeed(), actualMoveDir);
//            return;
//        }
//
//        // Get player tiles for next calculations
//        std::vector<Tile*> playerTiles = GameLogic::getPlayerTiles(player);
//
//        // Try to follow a pending direction change
//        if (dirChangeRequest != nullptr && dirChangeRequest->isPending()) {
//            MoveDir requestedDir = dirChangeRequest->getRequestedMoveDir();
//            if (GameLogic::changePlayerDirection(playerTiles, game.getFrametimeNormalizedSpeed(), requestedDir)) {
//                actualMoveDir = requestedDir;
//                delete dirChangeRequest;
//                dirChangeRequest = nullptr;
//                return;
//            }
//        }
//        // If every attempt fails, keep moving in the same direction
//        GameLogic::movePlayer(game.getFrametimeNormalizedSpeed(), actualMoveDir);
//
//    }
//}