#include "GameMenu.h"
#include <glft2/TextRenderer.hpp>
#include <GL/gl.h>

void GameMenu::render() {
    Game& game = Game::getInstance();
    glft2::font_data menuFont = game.getMenuFont();

    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    float screenW = (float)vp[2], screenH = (float)vp[3];
    float centerX = screenW * 0.5f, centerY = screenH * 0.5f;

    std::string gameName = "PacMan3D";
    std::string playText = "Play";
    std::string creditsText = "Credits";
    std::string exitText = "Exit";
    float titleScale = 0.7f, itemScale = 0.5f;

    float titleW, titleH;
    glft2::measureText(menuFont, gameName, &titleW, &titleH, titleScale);

    float hItem, wPlay, wCred, wExit;
    glft2::measureText(menuFont, playText, &wPlay, &hItem, itemScale);
    glft2::measureText(menuFont, creditsText, &wCred, &hItem, itemScale);
    glft2::measureText(menuFont, exitText, &wExit, &hItem, itemScale);

    float margin = hItem * 0.5f;
    float lineH = hItem + margin;
    float totalH = titleH + margin + 3 * lineH;
    float startY = centerY + totalH * 0.5f;

    glPushMatrix();
    glColor3ub(0, 0, 255);

    glft2::render2D(menuFont, centerX - titleW * 0.5f, startY, gameName, titleScale);
    glft2::render2D(menuFont, centerX - wPlay * 0.5f, startY - 1 * lineH, playText, itemScale);
    glft2::render2D(menuFont, centerX - wCred * 0.5f, startY - 2 * lineH, creditsText, itemScale);
    glft2::render2D(menuFont, centerX - wExit * 0.5f, startY - 3 * lineH, exitText, itemScale);

    glPopMatrix();
}
