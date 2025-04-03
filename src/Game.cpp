#include <GL/glut.h>
#include <iostream>
#include "Game.h"

Game* game = nullptr; // Global pointer for static methods

// Constructor body
Game::Game() {
    game = this;
    Player player = Player(0.0f, 0.0f);
    MapFactory mapFactory = MapFactory();
    map = mapFactory.createMap();
}

// Inits new game
void Game::init() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Register mouse callback functions
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);
    glutKeyboardFunc(keyboard);
}

// Mouse button callback (detects left button press, wheel)
void Game::mouseButton(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            // Mouse button is pressed
            game->isMousePressed = true;
            game->lastMouseX = x;
            game->lastMouseY = y;
        }
        else {
            // Mouse button is released
            game->isMousePressed = false;
        }
    }
    // Handle mouse wheel events
    else if (button == 3 || button == 4) {  // Button 3 is scroll up, Button 4 is scroll down
        if (state == GLUT_DOWN) {
            // Scroll up (zoom in)
            if (button == 3) {
                game->cameraDistance -= 2.0f; // Zoom in
            }
            // Scroll down (zoom out)
            else if (button == 4) {
                game->cameraDistance += 2.0f; // Zoom out
            }

            // Clamp camera distance to avoid too close or too far
            if (game->cameraDistance < 10.0f) game->cameraDistance = 10.0f;
            if (game->cameraDistance > 100.0f) game->cameraDistance = 100.0f;
        }
    }
}

// Mouse motion callback (tracks mouse movement)
void Game::mouseMotion(int x, int y) {
    if (game->isMousePressed) {
        // Calculate the change in mouse position
        int deltaX = x - game->lastMouseX;
        int deltaY = y - game->lastMouseY;

        // Update camera angles based on mouse movement
        game->cameraAngleY += deltaX * 0.1f;  // (horizontal rotation)
        game->cameraAngleX += deltaY * 0.1f;  // (vertical rotation)

        // Clamp vertical angle to prevent flipping
        if (game->cameraAngleX > 89.0f) game->cameraAngleX = 89.0f;
        if (game->cameraAngleX < -89.0f) game->cameraAngleX = -89.0f;

        // Update last mouse position
        game->lastMouseX = x;
        game->lastMouseY = y;
    }
}

void Game::keyboard(unsigned char key, int x, int y) {
    if (key == 'w') {
        game->moveDir = MoveDir::FWD;
    }
    if (key == 's') {
        game->moveDir = MoveDir::BWD;
    }
    if (key == 'd') {
        game->moveDir = MoveDir::RIGHT;
    }
    if (key == 'a') {
        game->moveDir = MoveDir::LEFT;
    }
}

// Update positions, handle logic
void Game::update(int value) {
    float newFrameTime = glutGet(GLUT_ELAPSED_TIME);
    float frameTimeSeconds = (newFrameTime - game->lastFrameTime) / 1000.0f;

    game->player.move(game->moveDir, frameTimeSeconds, game->moveSpeed);
    game->lastFrameTime = newFrameTime;

    // Trigger the display update by calling this to schedule a render
    glutPostRedisplay();

    glutTimerFunc(8, Game::update, 0);
}

// Render the game
void Game::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Calculate camera position based on angles and zoom (distance)
    float camX = game->cameraDistance * sin(-game->cameraAngleY * 3.14159f / 180.0f) * cos(game->cameraAngleX * 3.14159f / 180.0f);
    float camY = game->cameraDistance * sin(game->cameraAngleX * 3.14159f / 180.0f);
    float camZ = game->cameraDistance * cos(game->cameraAngleY * 3.14159f / 180.0f) * cos(game->cameraAngleX * 3.14159f / 180.0f);

    // Set the camera view
    gluLookAt(camX, camY, camZ,  // Eye position (based on camera angles and zoom)
        0.0, 0.0, 0.0,    // Look-at point (center of the map)
        0.0, 1.0, 0.0);   // Up vector

    // Render map
    game->map.render();
    game->player.render();

    glutSwapBuffers();
}

// Callback to trigger on glut window reshape
void Game::reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)w / (float)h, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}
