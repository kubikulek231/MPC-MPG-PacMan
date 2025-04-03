#include <GL/glut.h>
#include <iostream>
#include "Game.h"
#include "MapFactory.h"
#include "GameControl.h"
#include "GameLogic.h"

// Inits new game
void Game::init() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Register mouse callback functions
    glutMouseFunc(GameControl::mouseButton);
    glutMotionFunc(GameControl::mouseMotion);
    glutKeyboardFunc(GameControl::keyboard);
}

// Update positions, handle logic
void Game::update(int value) {
    Game& game = Game::getInstance();
    float newFrameTime = glutGet(GLUT_ELAPSED_TIME);
    float frameTimeSeconds = (newFrameTime - game.lastFrameTime) / 1000.0f;

    GameLogic::update(frameTimeSeconds);

    // Update the frametime
    game.setLastFrameTime(newFrameTime);

    // Trigger the display update by calling this to schedule a render
    glutPostRedisplay();

    glutTimerFunc(8, Game::update, 0);
}

// Render the game
void Game::render() {
    Game& game = Game::getInstance();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Calculate camera position based on angles and zoom (distance)
    float camX = game.cameraDistance * sin(-game.cameraAngleY * 3.14159f / 180.0f) * cos(game.cameraAngleX * 3.14159f / 180.0f);
    float camY = game.cameraDistance * sin(game.cameraAngleX * 3.14159f / 180.0f);
    float camZ = game.cameraDistance * cos(game.cameraAngleY * 3.14159f / 180.0f) * cos(game.cameraAngleX * 3.14159f / 180.0f);

    // Set the camera view
    gluLookAt(camX, camY, camZ,  // Eye position (based on camera angles and zoom)
        0.0, 0.0, 0.0,    // Look-at point (center of the map)
        0.0, 1.0, 0.0);   // Up vector

    // Render map
    game.map.render();
    game.player.render();

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
