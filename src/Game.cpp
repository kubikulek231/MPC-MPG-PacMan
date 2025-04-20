#include <GL/glut.h>
#include <iostream>
#include "MapFactory.h"
#include "Game.h"
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

    mapFactory = MapFactory();
    map = mapFactory.createMap();

    Point3D playerSpawnOrigin = map.getPlayerSpawn()->getOrigin();
    Point3D blinkySpawnOrigin = map.getBlinkySpawn()->getOrigin();
    Point3D pinkySpawnOrigin = map.getPinkySpawn()->getOrigin();
    Point3D inkySpawnOrigin = map.getInkySpawn()->getOrigin();
    Point3D clydeSpawnOrigin = map.getClydeSpawn()->getOrigin();

    player = Player(&map, playerSpawnOrigin, BoundingBox3D(Point3D(0, 0, 0), Point3D(0.999, 0.999, 0.999)));
    moveDir = MoveDir::NONE;
    pinky = Ghost(&map, pinkySpawnOrigin, BoundingBox3D(Point3D(0, 0, 0), Point3D(0.999, 0.999, 0.999)), "pinky");
    pinky.setColor(1.0, 0.5, 0.5);
    blinky = Ghost(&map, blinkySpawnOrigin, BoundingBox3D(Point3D(0, 0, 0), Point3D(0.999, 0.999, 0.999)), "blinky");
    blinky.setColor(1.0, 0.0, 0.0);
    inky = Ghost(&map, inkySpawnOrigin, BoundingBox3D(Point3D(0, 0, 0), Point3D(0.999, 0.999, 0.999)), "inky");
    inky.setColor(0.0, 1.0, 1.0);
    clyde = Ghost(&map, clydeSpawnOrigin, BoundingBox3D(Point3D(0, 0, 0), Point3D(0.999, 0.999, 0.999)), "clyde");
    clyde.setColor(1, 0.6, 0);

    ghosts.push_back(&pinky);
    ghosts.push_back(&blinky);
    ghosts.push_back(&inky);
    ghosts.push_back(&clyde);

    GameLogic::init();
}

// Update positions, handle logic
void Game::update(int value) {
    Game& game = Game::getInstance();
    float newFrameTimeSeconds = glutGet(GLUT_ELAPSED_TIME) / 1000.0f; // in s
    // Update the frametime
    game.lastFrameTimeDeltaSeconds = newFrameTimeSeconds - game.lastFrameTimeSeconds;
    game.lastFrameTimeSeconds = newFrameTimeSeconds;

    GameLogic::updatePlayer();
    GameLogic::updateGhosts();
    GameLogic::updateScore();
    GameLogic::updatePlayerLives();

    // Trigger the display update by calling this to schedule a render
    glutPostRedisplay();

    glutTimerFunc(8, Game::update, 0);
}

// Render the game
void Game::render() {
    Game& game = Game::getInstance();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    float radY = game.cameraAngleY * 3.14159f / 180.0f;
    float radX = game.cameraAngleX * 3.14159f / 180.0f;

    float camX = game.cameraPosX + game.cameraDistance * sin(-radY) * cos(radX);
    float camY = game.cameraDistance * sin(radX);
    float camZ = game.cameraPosZ + game.cameraDistance * cos(radY) * cos(radX);

    // Set the camera view
    gluLookAt(camX, camY, camZ,  // Eye position (based on camera angles and zoom)
        game.cameraPosX, 0.0, game.cameraPosZ,    // Look-at point (center of the map)
        0.0, 1.0, 0.0);   // Up vector

    // Render map
    game.getMap()->render(true);
    game.getPlayer()->render();

    // Render ghosts
    for (Ghost* ghost : game.getGhosts()) {
        ghost->render();
    }

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