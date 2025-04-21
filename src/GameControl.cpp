#include "GameControl.h"
#include "Game.h"
#include "MoveDir.h"

void GameControl::mouseButton(int button, int state, int x, int y) {
    Game& game = Game::getInstance();
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            game.setIsLeftMousePressed(true);
            game.setLastMouseX(x);
            game.setLastMouseY(y);
        }
        else {
            game.setIsLeftMousePressed(false);
        }
        return;
    }
    if (button == 3 || button == 4) {
        if (state == GLUT_DOWN) {
            if (button == 3) {
                game.setCameraDistance(game.getCameraDistance() - 2.0f);
            }
            else if (button == 4) {
                game.setCameraDistance(game.getCameraDistance() + 2.0f);
            }

            if (game.getCameraDistance() < 10.0f) game.setCameraDistance(10.0f);
            if (game.getCameraDistance() > 100.0f) game.setCameraDistance(100.0f);
        }
        return;
    }
    if (button == GLUT_MIDDLE_BUTTON) {
        if (state == GLUT_DOWN) {
            game.setIsMiddleMousePressed(true);
            game.setLastMouseX(x);
            game.setLastMouseY(y);
        }
        else {
            game.setIsMiddleMousePressed(false);
        }
        return;
    }
}

void GameControl::mouseMotion(int x, int y) {
    Game& game = Game::getInstance();
    if (game.getIsLeftMousePressed()) {
        int deltaX = x - game.getLastMouseX();
        int deltaY = y - game.getLastMouseY();

        game.setCameraAngleY(game.getCameraAngleY() + deltaX * 0.1f);
        game.setCameraAngleX(game.getCameraAngleX() + deltaY * 0.1f);

        if (game.getCameraAngleX() > 89.0f) game.setCameraAngleX(89.0f);
        if (game.getCameraAngleX() < -89.0f) game.setCameraAngleX(-89.0f);

        game.setLastMouseX(x);
        game.setLastMouseY(y);
        return;
    }
    if (game.getIsMiddleMousePressed()) {
        int deltaX = x - game.getLastMouseX();
        int deltaY = y - game.getLastMouseY();

        game.setCameraPosX(game.getCameraPosX() + deltaX * 0.01f);
        game.setCameraPosZ(game.getCameraPosZ() + deltaY * 0.01f);

        game.setLastMouseX(x);
        game.setLastMouseY(y);
        return;
    }
}

void GameControl::keyboard(unsigned char key, int x, int y) {
    Game& game = Game::getInstance();
    if (key == 'w') {
        game.setMoveDir(MoveDir::FWD);
        game.setIsDirectionKeyPressed(true);
    }
    if (key == 's') {
        game.setMoveDir(MoveDir::BWD);
        game.setIsDirectionKeyPressed(true);
    }
    if (key == 'd') {
        game.setMoveDir(MoveDir::RIGHT);
        game.setIsDirectionKeyPressed(true);
    }
    if (key == 'a') {
        game.setMoveDir(MoveDir::LEFT);
        game.setIsDirectionKeyPressed(true);
    }
    if (key == 'x') {
        game.setDebug1Pressed(true);
    }
    if (key == 'y') {
        game.setDebug2Pressed(true);
    }
}
