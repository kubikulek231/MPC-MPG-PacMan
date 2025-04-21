#ifndef GAMECONTROL_H
#define GAMECONTROL_H

class Game; // Forward declaration to avoid circular dependency

class GameControl {
public:
    static void mouseButton(int button, int state, int x, int y);
    static void mouseMotion(int x, int y);
    static void keyboard(unsigned char key, int x, int y);
};

#endif
