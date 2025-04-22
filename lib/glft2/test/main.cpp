#include "glfreetype/TextRenderer.hpp"
#include <GLFW/glfw3.h>

void GameMenu::render() {
    Game& game = Game::getInstance();
    glft2::font_data menuFont = game.getMenuFont();

    // Get current viewport dimensions
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    int screenWidth = viewport[2];
    int screenHeight = viewport[3];

    glPushMatrix();

    glColor3ub(0, 0, 0xff);

    float scale = 0.5f;
    float titleScale = 0.7f;
    float baseTextHeight;
    float titleTextHeight;

    float playTextWidth, creditsTextWidth, exitTextWidth, titleTextWidth;

    std::string gameName = "PacMan3D";
    std::string playText = "Play";
    std::string creditsText = "Credits";
    std::string exitText = "Exit";

    // Measure text
    glft2::measureText(menuFont, gameName, &titleTextWidth, &titleTextHeight, titleScale);
    glft2::measureText(menuFont, playText, &playTextWidth, &baseTextHeight, scale);
    glft2::measureText(menuFont, creditsText, &creditsTextWidth, &baseTextHeight, scale);
    glft2::measureText(menuFont, exitText, &exitTextWidth, &baseTextHeight, scale);

    float screenWidthCenter = screenWidth / 2.0f;
    float screenHeightCenter = screenHeight / 2.0f;

    float margin = baseTextHeight * 0.5f;

    // Total height = title + 3 entries + 3 margins
    float totalMenuHeight = titleTextHeight + 3 * baseTextHeight + 3 * margin;

    // Start Y position for top-center alignment
    float startY = screenHeightCenter - totalMenuHeight / 2.0f;

    // Render in the correct order: Title -> Play -> Credits -> Exit
    glft2::render2D(menuFont, screenWidthCenter - titleTextWidth / 2, startY, gameName, titleScale);
    glft2::render2D(menuFont, screenWidthCenter - playTextWidth / 2, startY - (baseTextHeight + margin), playText, scale);
    glft2::render2D(menuFont, screenWidthCenter - creditsTextWidth / 2, startY - 2 * (baseTextHeight + margin), creditsText, scale);
    glft2::render2D(menuFont, screenWidthCenter - exitTextWidth / 2, startY - 3 * (baseTextHeight + margin), exitText, scale);

    glPopMatrix();
}
