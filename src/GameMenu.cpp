#include "gl_includes.h"
#include "MoveDir.h"
#include <string>
#include <vector>
#include <iostream>
#include "Game.h"        // Make sure Game is included first
#include "MenuItem.h"    // Now MenuItem is properly defined
#include <glft2/TextRenderer.hpp>

GameMenu::GameMenu() {
}

void GameMenu::initMainMenu() {
    Game& game = Game::getInstance();
    glft2::font_data font = game.getMenuFont();
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    float screenW = float(vp[2]);
    float screenH = float(vp[3]);
    float centerX = screenW * 0.5f;
    float centerY = screenH * 0.5f;
    float textScale = 0.5f;

    // Get base item width and height (for the longest text)
    float maxTextW, maxTextH;
    glft2::measureText(font, "Exit to Main Menu", &maxTextW, &maxTextH, textScale);

    float itemPadding = 2.0f;
    float itemGap = 3.0f;

    // Calculate width and height of the menu items
    float itemW = maxTextW + itemPadding * 2.0f;
    float itemH = maxTextH + itemPadding * 2.0f;

    // Calculate the starting X for the first menu item to center it horizontally
    float firstItemX = centerX - itemW / 2.0f;

    // Calculate the total height of all the menu items plus the gaps
    float totalHeight = entries.size() * (itemH + itemGap) - itemGap;  // Total height of all items
    float firstItemY = centerY - totalHeight / 2.0f;  // Starting Y to center the menu vertically

    // Store the font pointer
    font_ptr = std::make_shared<glft2::font_data>(font);

    MenuItem play = MenuItem(font_ptr, "XD", firstItemX, firstItemY, itemW, itemH, textScale);
    MenuItem sandbox = MenuItem(font_ptr, "Sandbox", firstItemX, firstItemY + itemH + itemGap, itemW, itemH, textScale);
    MenuItem exit = MenuItem(font_ptr, "Exit", firstItemX, firstItemY + 2 * (itemH + itemGap), itemW, itemH, textScale);

    // Add the menu items to the entries
    setEntries({ play, sandbox, exit});
}


void GameMenu::initPauseMenu() {
    //setEntries({ "Resume", "Restart", "Exit to Main Menu" });
}

void GameMenu::setEntries(const std::vector<MenuItem>& items) {
    entries.clear();
    for (const auto& txt : items) {
        entries.emplace_back(txt);
    }
}

// Handle selecting of items
void GameMenu::update() {
    GameUserInput& guin = GameUserInput::getInstance();
    int mouseY = guin.getMouseY();
    int mouseX = guin.getMouseX();

    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    mouseY = vp[3] - mouseY;

    std::cout << "mouseY: " << mouseY << std::endl;
    std::cout << "mouseX: " << mouseX << std::endl;

    for (int i = 0; i < (int)entries.size(); ++i) {
        entries[i].update(mouseX, mouseY, false);
    }
}

void GameMenu::render() {
    Game& game = Game::getInstance();
    glft2::font_data font = game.getMenuFont();

    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    float screenW = float(vp[2]);
    float screenH = float(vp[3]);
    float centerX = screenW * 0.5f;
    float centerY = screenH * 0.5f;

    std::string title = "PacMan3D";
    float titleScale = 0.7f;
    float titleW, titleH;
    glft2::measureText(font, title, &titleW, &titleH, titleScale);

    float itemScale = 0.5f;

    // Get entry height
    float _, itemH;
    glft2::measureText(font, "XYHEIGHT", &_, &itemH, itemScale);

    float margin = itemH * 0.5f;
    float lineH = itemH + margin;

    float totalH = titleH + margin + entries.size() * lineH;
    float startY = centerY + totalH * 0.5f;

    // Render title
    glPushMatrix();
    glColor3ub(0, 0, 255);
    
    glft2::render2D(font, centerX - titleW * 0.5f, startY, title, titleScale);

    glPopMatrix();

    float currentY = startY;
    for (int i = 0; i < (int)entries.size(); ++i) {
        currentY = currentY - itemH - margin;
        float currentX = centerX - entries[i].getWidth() / 2;
        entries[i].updateXY(currentX, currentY);
        entries[i].render();
    }
}
