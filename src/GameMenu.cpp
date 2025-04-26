#include "GameMenu.h"
#include "Game.h"
#include <glft2/TextRenderer.hpp>
#include <GL/gl.h>

GameMenu::GameMenu() {
    initMainMenu();
}

void GameMenu::initMainMenu() {
    setEntries({ "Play", "Sandbox", "Credits", "Exit" });
}

void GameMenu::initPauseMenu() {
    setEntries({ "Resume", "Restart", "Exit to Main Menu" });
}

void GameMenu::setEntries(const std::vector<std::string>& items) {
    entries.clear();
    for (const auto& txt : items) {
        entries.emplace_back(txt);
    }
    if (!entries.empty()) {
        entries[0].selected = true;
    }
}

void GameMenu::setSelectedIndex(int idx) {
    if (idx < 0 || idx >= (int)entries.size()) return;
    for (int i = 0; i < (int)entries.size(); ++i) {
        entries[i].selected = (i == idx);
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
    float selScale = 0.6f;
    float dummyW, itemH;
    if (!entries.empty()) {
        glft2::measureText(font, entries[0].text, &dummyW, &itemH, itemScale);
    }
    float margin = itemH * 0.5f;
    float lineH = itemH + margin;

    float totalH = titleH + margin + entries.size() * lineH;
    float startY = centerY + totalH * 0.5f;

    glPushMatrix();
    glColor3ub(0, 0, 255);
    glft2::render2D(font, centerX - titleW * 0.5f, startY, title, titleScale);

    for (int i = 0; i < (int)entries.size(); ++i) {
        const auto& e = entries[i];
        float scale = e.selected ? selScale : itemScale;
        float w, h;
        glft2::measureText(font, e.text, &w, &h, scale);
        float x = centerX - w * 0.5f;
        float y = startY - titleH - margin - i * lineH;
        if (e.selected) glColor3ub(255, 255, 255);
        else            glColor3ub(0, 0, 255);
        glft2::render2D(font, x, y, e.text, scale);
    }

    glPopMatrix();
}
