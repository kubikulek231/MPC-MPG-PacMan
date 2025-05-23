#ifndef GAMEMENU_H
#define GAMEMENU_H

#include "gl_includes.h"
#include "MoveDir.h"
#include <string>
#include <vector>
#include "Game.h"
#include "MenuItem.h"

class GameMenu {
public:
    // Initialize default (main) menu
    GameMenu();
    // Initialize main menu entries
    void initMainMenu();
    // Initialize pause menu entries
    void initPauseMenu();
    // Initialize gane menu entries
    void initGameOverMenu();

    void setUserScore(int score);
    
    std::string getEnteredMenuItemString();
    void update();
    void setEntries(const std::vector<MenuItem>& items);
    void render();

private:
    void renderMenuBackground();
    int selectedId = -1;
    std::vector<MenuItem> entries;
    std::shared_ptr<glft2::font_data> font_ptr;
};

#endif
