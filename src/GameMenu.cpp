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

std::string GameMenu::getEnteredMenuItemString() {
    for (const auto& entry : entries) {
        if (entry.isEntered()) { 
            return entry.getText(); 
        }
    }
    return std::string("");
}

void GameMenu::setUserScore(int score) {
    for (auto& entry : entries) {
        // Compare against the text prefix:
        if (entry.getText().rfind("Your Score:", 0) == 0) {
            // Convert score to string and concatenate:
            std::string scoreString = "Your Score: " + std::to_string(score);
            entry.setText(scoreString);
            return;
        }
    }
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

    float itemW = maxTextW + itemPadding * 2.0f;
    float itemH = maxTextH + itemPadding * 2.0f;

    float firstItemX = centerX - itemW / 2.0f;

    // Store the font pointer
    font_ptr = std::make_shared<glft2::font_data>(font);

    MenuItem play = MenuItem(font_ptr, "Play", firstItemX, 0, itemW, itemH, textScale);
    MenuItem sandbox = MenuItem(font_ptr, "Sandbox", firstItemX, 0 + itemH + itemGap, itemW, itemH, textScale);
    MenuItem exit = MenuItem(font_ptr, "Exit", firstItemX, 0 + 2 * (itemH + itemGap), itemW, itemH, textScale);

    // Add the menu items to the entries
    setEntries({ play, sandbox, exit});
}


void GameMenu::initPauseMenu() {
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

    float itemW = maxTextW + itemPadding * 2.0f;
    float itemH = maxTextH + itemPadding * 2.0f;

    float firstItemX = centerX - itemW / 2.0f;

    // Store the font pointer
    font_ptr = std::make_shared<glft2::font_data>(font);

    MenuItem play = MenuItem(font_ptr, "Resume", firstItemX, 0, itemW, itemH, textScale);
    MenuItem exitToMenu = MenuItem(font_ptr, "Exit to Main Menu", firstItemX, 0 + 2 * (itemH + itemGap), itemW, itemH, textScale);
    MenuItem exit = MenuItem(font_ptr, "Exit", firstItemX, 0 + 2 * (itemH + itemGap), itemW, itemH, textScale);

    // Add the menu items to the entries
    setEntries({ play, exitToMenu, exit });
}

void GameMenu::initGameOverMenu() {
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

    float itemW = maxTextW + itemPadding * 2.0f;
    float itemH = maxTextH + itemPadding * 2.0f;

    float firstItemX = centerX - itemW / 2.0f;

    // Store the font pointer
    font_ptr = std::make_shared<glft2::font_data>(font);

    MenuItem gameOverTitle = MenuItem(font_ptr, "Game Over!", firstItemX, 0, itemW, itemH, textScale);
    gameOverTitle.setSelectable(false);
    MenuItem score = MenuItem(font_ptr, "Your Score: 9999", firstItemX, 0, itemW, itemH, textScale);
    score.setSelectable(false);
    MenuItem exitToMenu = MenuItem(font_ptr, "Exit to Main Menu", firstItemX, 0 + 2 * (itemH + itemGap), itemW, itemH, textScale);
    MenuItem exit = MenuItem(font_ptr, "Exit", firstItemX, 0 + 2 * (itemH + itemGap), itemW, itemH, textScale);

    // Add the menu items to the entries
    setEntries({ gameOverTitle, score, exitToMenu, exit });
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
    bool isEntered = false;
    if (guin.isButtonFlagPressedAndReleased(GLUT_LEFT_BUTTON)) {
        guin.resetButtonFlagPressedAndReleased(GLUT_LEFT_BUTTON);
        isEntered = true;
    }

    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    mouseY = vp[3] - mouseY;

    for (int i = 0; i < (int)entries.size(); ++i) {
        entries[i].update(mouseX, mouseY, isEntered);
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

    renderMenuBackground();

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
    glColor3ub(255, 255, 0);
    
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

void GameMenu::renderMenuBackground() {
    Game& game = Game::getInstance();
    glft2::font_data font = game.getMenuFont();

    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    float screenW = float(vp[2]);
    float screenH = float(vp[3]);
    float centerX = screenW * 0.5f;
    float centerY = screenH * 0.5f;

    // Setup orthographic projection 
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();               // save 3D projection
    glLoadIdentity();
    // note: origin bottom-left; if you want top-left, swap the last two args
    glOrtho(0.0, screenW,
        0.0, screenH,
        -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();             // save 3D modelview
    glLoadIdentity();

    // Draw semi-transparent overlay
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
    glBegin(GL_QUADS);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(screenW, 0.0f);
    glVertex2f(screenW, screenH);
    glVertex2f(0.0f, screenH);
    glEnd();

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    glPopMatrix();              // restore 3D modelview
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();                // restore 3D projection
    glMatrixMode(GL_MODELVIEW);   // back to modelview for the rest
}