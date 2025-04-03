#include <GL/glut.h>
#include "Game.h"

Game* gameInstance = nullptr; // Global pointer for static methods

// Constructor body
Game::Game() {
    gameInstance = this;
    map = MapFactory::createMap();
    player = Player(0.0f, 0.0f);
    cameraDistance = 50.0f;  // Initial camera distance
}

// Inits new game
void Game::init() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Register mouse callback functions
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);
}

// Mouse button callback (detects left button press, wheel)
void Game::mouseButton(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            // Mouse button is pressed
            gameInstance->isMousePressed = true;
            gameInstance->lastMouseX = x;
            gameInstance->lastMouseY = y;
        }
        else {
            // Mouse button is released
            gameInstance->isMousePressed = false;
        }
    }
    // Handle mouse wheel events
    else if (button == 3 || button == 4) {  // Button 3 is scroll up, Button 4 is scroll down
        if (state == GLUT_DOWN) {
            // Scroll up (zoom in)
            if (button == 3) {
                gameInstance->cameraDistance -= 2.0f; // Zoom in
            }
            // Scroll down (zoom out)
            else if (button == 4) {
                gameInstance->cameraDistance += 2.0f; // Zoom out
            }

            // Clamp camera distance to avoid too close or too far
            if (gameInstance->cameraDistance < 10.0f) gameInstance->cameraDistance = 10.0f;
            if (gameInstance->cameraDistance > 100.0f) gameInstance->cameraDistance = 100.0f;
        }
    }
}

// Mouse motion callback (tracks mouse movement)
void Game::mouseMotion(int x, int y) {
    if (gameInstance->isMousePressed) {
        // Calculate the change in mouse position
        int deltaX = x - gameInstance->lastMouseX;
        int deltaY = y - gameInstance->lastMouseY;

        // Update camera angles based on mouse movement
        gameInstance->cameraAngleY += deltaX * 0.1f;  // Adjust for sensitivity (horizontal rotation)
        gameInstance->cameraAngleX += deltaY * 0.1f;  // Adjust for sensitivity (vertical rotation)

        // Clamp vertical angle to prevent flipping
        if (gameInstance->cameraAngleX > 89.0f) gameInstance->cameraAngleX = 89.0f;
        if (gameInstance->cameraAngleX < -89.0f) gameInstance->cameraAngleX = -89.0f;

        // Update last mouse position
        gameInstance->lastMouseX = x;
        gameInstance->lastMouseY = y;
    }
}

// Update the render to apply the rotation and zoom
void Game::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Calculate camera position based on angles and zoom (distance)
    float camX = cameraDistance * sin(-cameraAngleY * 3.14159f / 180.0f) * cos(cameraAngleX * 3.14159f / 180.0f);
    float camY = cameraDistance * sin(cameraAngleX * 3.14159f / 180.0f);
    float camZ = cameraDistance * cos(cameraAngleY * 3.14159f / 180.0f) * cos(cameraAngleX * 3.14159f / 180.0f);

    // Set the camera view
    gluLookAt(camX, camY, camZ,  // Eye position (based on camera angles and zoom)
        0.0, 0.0, 0.0,    // Look-at point (center of the map)
        0.0, 1.0, 0.0);   // Up vector

    // Render map
    map.render();
    // Render player
    player.render();

    glutSwapBuffers();
}

// Callback to trigger on glut display
void Game::display() {
    if (gameInstance) {
        gameInstance->render();
    }
}

// Callback to trigger on glut window reshape
void Game::reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)w / (float)h, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}
