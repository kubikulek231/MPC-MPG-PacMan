#include "GameMenu.h"
#include <glft2/TextRenderer.hpp>

void GameMenu::render() {
    Game& game = Game::getInstance();

    glPushMatrix();

    // Blue text
    glColor3ub(0, 0, 0xff);

    glft2::render2D(game.getGameFont(), 20, 20, "START GAME");

    glPopMatrix();

}