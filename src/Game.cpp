#include "glft2/TextRenderer.hpp"
#include <iostream>
#include "MapFactory.h"
#include "Game.h"
#include "GameControl.h"
#include "GameLogic.h"
#include "GameCamera.h"
#include "GameMenu.h"
#include "math.h"
#include <sstream>     
#include <string>      
#include <vector>      

// Global wrapper functions to be passed to GLUT
static void keyboardCallback(unsigned char key, int x, int y) { GameUserInput::getInstance().keyboard(tolower(key), x, y); }
static void keyboardUpCallback(unsigned char key, int x, int y) { GameUserInput::getInstance().keyboardUp(tolower(key), x, y); }
static void mouseButtonCallback(int button, int state, int x, int y) { GameUserInput::getInstance().mouseButton(button, state, x, y); }
static void mouseMotionCallback(int x, int y) { GameUserInput::getInstance().mouseMotion(x, y); }


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
    playerLives = 3;

    gameFont.init("assets/fonts/Roboto-Regular.ttf", 128);
    menuFont.init("assets/fonts/Roboto-Regular.ttf", 72);
    Game::initLevel();
    Game& game = getInstance();
    glft2::font_data font = game.getMenuFont();
    // Preload main menu
    game.gameMenu.initMainMenu();
}

void Game::initLevel(int level) {
    Game& game = getInstance();
    game.moveDir = MoveDir::NONE;
    if (level < 0) { level = getCurrentLevel(); }
    mapFactory = MapFactory();
    map = mapFactory.createMap();
    GameControl& gc = GameControl::getInstance();
    // Press and release movement key to start the level
    gc.enableWasdAfterFullPressCycle();
    // Reset moveDir
    gc.setMoveDir(MoveDir::NONE);

    Point3D playerSpawnOrigin = map.getPlayerSpawn()->getOrigin();
    Point3D blinkySpawnOrigin = map.getBlinkySpawn()->getOrigin();
    Point3D pinkySpawnOrigin = map.getPinkySpawn()->getOrigin();
    Point3D inkySpawnOrigin = map.getInkySpawn()->getOrigin();
    Point3D clydeSpawnOrigin = map.getClydeSpawn()->getOrigin();

    playerSpawnOrigin.move(MapFactory::TILE_SIZE / 2.0f, 0.0f, 0.0f);
    blinkySpawnOrigin.move(- MapFactory::TILE_SIZE / 2.0f, 0.0f, 0.0f);
    pinkySpawnOrigin.move(- MapFactory::TILE_SIZE / 2.0f, 0.0f, 0.0f);
    inkySpawnOrigin.move(- MapFactory::TILE_SIZE / 2.0f, 0.0f, 0.0f);
    clydeSpawnOrigin.move(- MapFactory::TILE_SIZE / 2.0f, 0.0f, 0.0f);

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

// First interface to handle game logic
void Game::update(int value) {
    Game& game = Game::getInstance();
    GameCamera& gcam = GameCamera::getInstance();
    GameUserInput& guin = GameUserInput::getInstance();

    float newFrameTimeS = glutGet(GLUT_ELAPSED_TIME) / 1000.0f; // in s
    
    // Update the frametime
    game.lastFrameTimeDeltaS = newFrameTimeS - game.lastFrameTimeS;
    game.lastFrameTimeS = newFrameTimeS;

    // HANDLE GAME LOGIC
    // Update the game logic and control only if playing
    if (game.gameState == GameState::Playing) { 
        if (guin.isKeyFlagPressed('\x1B')) {
            guin.resetKeyFlagPressed('\x1B');
            game.gameState = GameState::Paused;
            game.gameMenu.initPauseMenu();
        }

        // Unlock the user camera movement
        gcam.setLockUserUpdate(false);

        GameLogic::updatePlayer();
        GameLogic::updateGhosts();
        GameLogic::updateScore();
        GameLogic::updatePlayerLives();

        GameControl& gcon = GameControl::getInstance();

        // Update user input based logic
        gcon.update();

        if (game.getPlayerLives() < 0) {
            game.gameState = GameState::GameOver;
            game.gameMenu.initGameOverMenu();
            game.gameMenu.setUserScore(game.getTotalScore());
        }
    }

    // HANDLE MENU LOGIC
    if (game.gameState != GameState::Playing) {
        // Lock user camera updates when not in game
        gcam.setLockUserUpdate(true);
        // Render the menu only when not playing
        game.gameMenu.update();
        std::string enteredItem = game.gameMenu.getEnteredMenuItemString();
        if (enteredItem == "Play") {
            game.gameState = GameState::Playing;
        }
        if (enteredItem == "Sandbox") {
            game.playerLives = 9999;
            game.gameState = GameState::Playing;
        }
        if (enteredItem == "Exit") {
            exit(0);
        }
        if (enteredItem == "Resume") {
            // Flush the escape flag just in case
            guin.resetKeyFlagPressed('\x1B');
            // Resume
            game.gameState = GameState::Playing;
        }
        if (enteredItem == "Exit to Main Menu") {
            game.gameState = GameState::MainMenu;
            game.gameMenu.initMainMenu();
            // Reset level and init new one
            game.currentLevel = 0;
            game.playerLives = 3;
            game.totalScore = 0;
            game.initLevel();
        }
    }

    // Handle esc to resume
    if (game.gameState == GameState::Paused) {
        if (guin.isKeyFlagPressed('\x1B')) {
            guin.resetKeyFlagPressed('\x1B');
            game.gameState = GameState::Playing;
        }
    }

    // Always update the camera
    gcam.update(game.lastFrameTimeDeltaS);

    // Trigger the display update by calling this to schedule a render
    glutPostRedisplay();
    glutTimerFunc(8, Game::update, 0);
}

void Game::render() {
    Game& game = Game::getInstance();
    GameCamera& gcam = GameCamera::getInstance();
    CameraGlu cam = gcam.getCameraGLU();

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
    game.getMap()->render(false);
    game.getPlayer()->render();
    for (Ghost* ghost : game.getGhosts()) { ghost->render(); }

    if (game.gameState == GameState::Playing) {
        game.renderScore();
        game.renderLives();
        game.renderCameraInfo();
    }
    
    if (game.gameState != GameState::Playing) {
        game.gameMenu.render();
    }

    glutSwapBuffers();
}


// Callback to trigger on glut window reshape
void Game::reshape(int w, int h) {
    const int MIN_W = 800;
    const int MIN_H = 600;

    // Enforce minimum window size
    if (w < MIN_W || h < MIN_H) {
        glutReshapeWindow(
            std::max(w, MIN_W),
            std::max(h, MIN_H)
        );
        return;
    }

    // Normal viewport/projection setup
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, float(w) / float(h), 0.1, 100.0);

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

void Game::renderCameraInfo() {
    Game& game = Game::getInstance();

    // one-time timer start
    static bool started = false;
    if (!started) {
        game.cameraHintFadeTimer.configure(0, 10000, 1000);
        game.cameraHintFadeTimer.start();
        started = true;
    }

    game.cameraHintFadeTimer.update(std::chrono::steady_clock::now());
    float alpha = game.cameraHintFadeTimer.getAlpha();
    if (alpha <= 0.0f) return;

    auto& cam = GameCamera::getInstance();
    const std::string modeText = cam.getCameraModeString();
    const std::string hintText = "Change camera by pressing 'C' key.";
    const std::string hintMouse = "Adjust by mouse.";

    glft2::font_data font;
    font = game.getMenuFont();
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    float W = float(vp[2]), H = float(vp[3]);
    float marginX = W * 0.02f;
    float marginY = H * 0.02f;

    // measure all three
    const float hintScale = 0.3f;
    float hintW, hintH;
    glft2::measureText(font, hintText, &hintW, &hintH, hintScale);
    float mouseW, mouseH;
    glft2::measureText(font, hintMouse, &mouseW, &mouseH, hintScale);

    const float modeScale = 0.4f;
    float modeW, modeH;
    glft2::measureText(font, modeText, &modeW, &modeH, modeScale);

    // vertical layout, bottom-up:
    float spacing = hintH * 0.5f;
    float yMouse = marginY;
    float yHint = yMouse + mouseH + spacing;
    float yMode = yHint + hintH + spacing;

    // each line right-aligned
    float xMouse = W - mouseW - marginX;
    float xHint = W - hintW - marginX;
    float xMode = W - modeW - marginX;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1, 1, 1, alpha);
    glPushMatrix();
    glft2::render2D(font, xMode, yMode, modeText, modeScale);
    glft2::render2D(font, xHint, yHint, hintText, hintScale);
    glft2::render2D(font, xMouse, yMouse, hintMouse, hintScale);
    glPopMatrix();
    glDisable(GL_BLEND);
}
