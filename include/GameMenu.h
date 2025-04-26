#ifndef GAMEMENU_H
#define GAMEMENU_H

#include "gl_includes.h"
#include "MoveDir.h"
#include <string>
#include <vector>

// Represents a single menu entry with a label and selection state
class MenuEntry {
public:
    MenuEntry(const std::string& txt)
        : text(txt), selected(false) {
    }

    std::string text;
    bool        selected;
};


class GameMenu {
public:
    // Initialize default (main) menu
    GameMenu();
    // Initialize main menu entries
    void initMainMenu();
    // Initialize pause menu entries
    void initPauseMenu();
    void setEntries(const std::vector<std::string>& items);
    void setSelectedIndex(int idx);
    void render();

private:
    std::vector<MenuEntry> entries;
};

#endif
