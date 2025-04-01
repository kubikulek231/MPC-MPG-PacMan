#ifndef GAME_H
#define GAME_H

#include "Map.h"
#include "MapFactory.h"

class Game {
public:
    Game();
    void init();    // Initialize OpenGL settings
    void update();  // Update game logic
    void render();  // Display the game scene

    // Camera rotation angles
    float cameraDistance = 50.0f;
    float cameraAngleX = 0.0f;  // Rotation around the X-axis (pitch)
    float cameraAngleY = 0.0f;  // Rotation around the Y-axis (yaw)
    bool isMousePressed = false; // To check if the left mouse button is pressed
    int lastMouseX, lastMouseY;  // To track the last mouse position
    
    // GLUT Callbacks
    static void display();
    static void reshape(int w, int h);

    // Mouse callback functions
    static void mouseButton(int button, int state, int x, int y);
    static void mouseMotion(int x, int y);
    static void mouseWheel(int button, int state, int x, int y);
private:
    Map map;  // Store the game map
};

#endif // GAME_H
