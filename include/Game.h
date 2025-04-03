#ifndef GAME_H
#define GAME_H

#include "Map.h"
#include "MapFactory.h"
#include "Player.h"

class Game {
public:
    Game();
    void init();    // Initialize OpenGL settings
    static void update(int value = 0);  // Update game logic
    static void render();  // Display the game scene
    
    // GLUT Callbacks
    static void reshape(int w, int h);

    // Mouse callback functions
    static void mouseButton(int button, int state, int x, int y);
    static void mouseMotion(int x, int y);
    static void keyboard(unsigned char key, int x, int y);

private:
    Map map;
    Player player;
    MoveDir moveDir;
    float lastFrameTime = 0.0f;
    float cameraDistance = 50.0f;
    float cameraAngleX = 0.0f;  // Rotation around the X-axis (pitch)
    float cameraAngleY = 0.0f;  // Rotation around the Y-axis (yaw)
    bool isMousePressed = false; // To check if the left mouse button is pressed
    int lastMouseX, lastMouseY;  // To track the last mouse position
    float moveSpeed = 1.5f;
};

#endif // GAME_H
