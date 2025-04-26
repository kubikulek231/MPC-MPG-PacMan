#ifndef GAMEMENU_H
#define GAMEMENU_H

#include "gl_includes.h"
#include "MoveDir.h"
#include <string>
#include <vector>

class MenuEntry {
public:
    MenuEntry(const std::string& txt)
        : text(txt), selected(false), x(0), y(0), width(0), height(0) {
    }

    std::string text;
    bool        selected;

    // Bounding box
    float x, y, width, height;

    // Method to check if a given point (px, py) intersects with the bounding box
    bool intersects(float px, float py) const {
        // Check if the point (px, py) is inside the bounding box
        if (px >= x && px <= x + width && py >= y && py <= y + height) {
            return true;  // Inside the bounding box
        }
        return false;  // Outside the bounding box
    }
};


class GameMenu {
public:
    // Initialize default (main) menu
    GameMenu();
    // Initialize main menu entries
    void initMainMenu();
    // Initialize pause menu entries
    void initPauseMenu();
    
    void selectPrev();
    void selectNext();
    void update();
    void setEntries(const std::vector<std::string>& items);
    void setSelectedIndex(int idx);
    void render();

private:
    std::vector<MenuEntry> entries;
};

#endif
