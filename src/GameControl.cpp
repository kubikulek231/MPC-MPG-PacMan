#include "GL/glut.h"

#include "GameControl.h"
#include "Game.h"
#include "MoveDir.h"

void GameControl::mouseButton(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        Game& game = Game::getInstance();
        if (state == GLUT_DOWN) {
            game.setIsMousePressed(true);
            game.setLastMouseX(x);
            game.setLastMouseY(y);
        }
        else {
            game.setIsMousePressed(false);
        }
    }
    else if (button == 3 || button == 4) {
        Game& game = Game::getInstance();
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
    }
}

void GameControl::mouseMotion(int x, int y) {
    Game& game = Game::getInstance();
    if (game.getIsMousePressed()) {
        int deltaX = x - game.getLastMouseX();
        int deltaY = y - game.getLastMouseY();

        game.setCameraAngleY(game.getCameraAngleY() + deltaX * 0.1f);
        game.setCameraAngleX(game.getCameraAngleX() + deltaY * 0.1f);

        if (game.getCameraAngleX() > 89.0f) game.setCameraAngleX(89.0f);
        if (game.getCameraAngleX() < -89.0f) game.setCameraAngleX(-89.0f);

        game.setLastMouseX(x);
        game.setLastMouseY(y);
    }
}

void GameControl::keyboard(unsigned char key, int x, int y) {
    Game& game = Game::getInstance();
    if (key == 'w') {
        game.setMoveDir(MoveDir::FWD);
        game.setIsDirectionChanged(true);
    }
    if (key == 's') {
        game.setMoveDir(MoveDir::BWD);
        game.setIsDirectionChanged(true);
    }
    if (key == 'd') {
        game.setMoveDir(MoveDir::RIGHT);
        game.setIsDirectionChanged(true);
    }
    if (key == 'a') {
        game.setMoveDir(MoveDir::LEFT);
        game.setIsDirectionChanged(true);
    }
}
