#include "glft2/TextRenderer.hpp"
#include <iostream>
#include "MapFactory.h"
#include "Game.h"
#include "GameControl.h"
#include "GameLogic.h"
#include "GameMenu.h"
#include "math.h"
#include <sstream>     
#include <string>      
#include <vector>      

#define PI 3.14159265358979323846f

// Global wrapper functions to be passed to GLUT
static void keyboardCallback(unsigned char key, int x, int y) { GameControl::getInstance().keyboard(tolower(key), x, y); }
static void keyboardUpCallback(unsigned char key, int x, int y) { GameControl::getInstance().keyboardUp(tolower(key), x, y); }
static void mouseButtonCallback(int button, int state, int x, int y) { GameControl::getInstance().mouseButton(button, state, x, y); }
static void mouseMotionCallback(int x, int y) { GameControl::getInstance().mouseMotion(x, y); }


// Inits new game
void Game::init() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Register mouse callback functions
    glutMouseFunc(mouseButtonCallback);
    glutPassiveMotionFunc(mouseMotionCallback);
    glutMotionFunc(mouseMotionCallback);
    glutKeyboardFunc(keyboardCallback);
    glutKeyboardUpFunc(keyboardUpCallback);

    currentLevel = 0;
    playerLives = 100;

    Game::initLevel();
    gameFont.init("assets/fonts/Roboto-Regular.ttf", 128);
    menuFont.init("assets/fonts/Roboto-Regular.ttf", 72);
}

void Game::initLevel(int level) {
    Game& game = getInstance();
    game.moveDir = MoveDir::NONE;

    if (level < 0) { level = getCurrentLevel(); }
    mapFactory = MapFactory();
    map = mapFactory.createMap();

    Point3D playerSpawnOrigin = map.getPlayerSpawn()->getOrigin();
    Point3D blinkySpawnOrigin = map.getBlinkySpawn()->getOrigin();
    Point3D pinkySpawnOrigin = map.getPinkySpawn()->getOrigin();
    Point3D inkySpawnOrigin = map.getInkySpawn()->getOrigin();
    Point3D clydeSpawnOrigin = map.getClydeSpawn()->getOrigin();

    player = Player(&map, playerSpawnOrigin, BoundingBox3D(Point3D(0, 0, 0), Point3D(0.999, 0.999, 0.999)));
    
    float levelSpeed = game.getBaseSpeed() + level * LEVEL_SPEED_INCREMENT;
    float ghostSpeed = levelSpeed * (1 + GHOST_SPEED_COMP);

    uint64_t blinkDurationMs = Player::DEFAULT_BLINK_DURATION_MS * std::pow(LEVEL_DURATION_MULTIPLIER, level);
    uint64_t dirChangeRequestExpireAfterMs = Player::DEFAULT_DIR_CHANGE_REQUEST_EXPIRE_AFTER_MS * std::pow(LEVEL_DURATION_MULTIPLIER, level);
    uint64_t invincibleExpireAfterMs = Player::DEFAULT_INVINCIBLE_EXPIRE_AFTER_MS * std::pow(LEVEL_DURATION_MULTIPLIER, level);

    player.setMoveSpeed(levelSpeed);
    player.setDirChangeRequestExpireAfterMs(dirChangeRequestExpireAfterMs);
    player.setBlinkDuration(blinkDurationMs);
    player.setInvincibleExpireAfterMs(invincibleExpireAfterMs);

    pinky = Ghost(&map, pinkySpawnOrigin, BoundingBox3D(Point3D(0, 0, 0), Point3D(0.999, 0.999, 0.999)), "pinky");
    pinky.setColor(1.0, 0.5, 0.5);
    pinky.setMoveSpeed(ghostSpeed);
    blinky = Ghost(&map, blinkySpawnOrigin, BoundingBox3D(Point3D(0, 0, 0), Point3D(0.999, 0.999, 0.999)), "blinky");
    blinky.setColor(1.0, 0.0, 0.0);
    blinky.setMoveSpeed(ghostSpeed);
    inky = Ghost(&map, inkySpawnOrigin, BoundingBox3D(Point3D(0, 0, 0), Point3D(0.999, 0.999, 0.999)), "inky");
    inky.setColor(0.0, 1.0, 1.0);
    inky.setMoveSpeed(ghostSpeed);
    clyde = Ghost(&map, clydeSpawnOrigin, BoundingBox3D(Point3D(0, 0, 0), Point3D(0.999, 0.999, 0.999)), "clyde");
    clyde.setColor(1, 0.6, 0);
    clyde.setMoveSpeed(ghostSpeed);
    
    ghosts.clear();

    ghosts.push_back(&pinky);
    ghosts.push_back(&blinky);
    ghosts.push_back(&inky);
    ghosts.push_back(&clyde);

    GameLogic::initLevel();
}

// Update positions, handle logic
void Game::update(int value) {
    Game& game = Game::getInstance();

    float newFrameTimeS = glutGet(GLUT_ELAPSED_TIME) / 1000.0f; // in s
    // Update the frametime
    game.lastFrameTimeDeltaS = newFrameTimeS - game.lastFrameTimeS;
    game.lastFrameTimeS = newFrameTimeS;

    GameLogic::updatePlayer();
    GameLogic::updateGhosts();
    GameLogic::updateScore();
    GameLogic::updatePlayerLives();
    game.getGameControlInstance().update(game.lastFrameTimeDeltaS);

    // Trigger the display update by calling this to schedule a render
    glutPostRedisplay();

    glutTimerFunc(8, Game::update, 0);
}

void Game::render() {
    Game& game = Game::getInstance();
    GameControl& gc = GameControl::getInstance();
    CameraGlu cam = gc.getCameraGLU();


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(
        cam.posX,       // Camera Position X
        cam.posY,       // Camera Position Y
        cam.posZ,       // Camera Position Z
        cam.lookAtX,    // LookAt X
        cam.lookAtY,    // LookAt Y
        cam.lookAtZ,    // LookAt Z
        cam.upX, cam.upY, cam.upZ         // Up Vector
    );

    // Render game elements
    game.getMap()->render(true);
    game.getPlayer()->render();
    game.renderScore();
    game.renderLives();
    GameMenu::render();

    for (Ghost* ghost : game.getGhosts()) { ghost->render(); }

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

void Game::renderScore() {
    Game& game = Game::getInstance();
    Map* map = game.getMap();
    Tile* tile = map->getTileAt(1, 1); // Get the target tile

    Point3D textOrigin = tile->getOrigin(); // Get the 3D position of the tile

    std::string scoreText = "Total score: " + std::to_string(game.getTotalScore());

    glPushMatrix();

    glColor3f(1.0f, 1.0f, 1.0f);

    float scale = 0.008f;
    float textWidth;
    float textHeight;
    glft2::measureText(game.gameFont, scoreText, &textWidth, &textHeight, scale);

    // Move to the text's origin
    glTranslatef(0 - textWidth / 2, textOrigin.y + 0.01, textOrigin.z + (1 * MapFactory::TILE_SIZE));

    // Single rotation to orient the text
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);

    glft2::render3D(game.gameFont, scoreText, scale);

    glPopMatrix();
}

void Game::renderLives() {
    Game& game = Game::getInstance();
    Map* map = game.getMap();
    Tile* tile = map->getTileAt(MapFactory::MAP_HEIGHT - 1, 1); // Get the target tile

    Point3D textOrigin = tile->getOrigin(); // Get the 3D position of the tile

    std::string livesText = "Remaining lives: " + std::to_string(game.getPlayerLives());

    glPushMatrix();

    glColor3f(1.0f, 1.0f, 1.0f);

    // Move to the text's origin
    glTranslatef(textOrigin.x, textOrigin.y + 0.01, textOrigin.z + MapFactory::TILE_SIZE / 2);

    // Single rotation to orient the text
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);

    float scale = 0.008f;
    glft2::render3D(game.gameFont, livesText, scale);
    
    glPopMatrix();
}