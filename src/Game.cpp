#include "glft2/TextRenderer.hpp"
#include <iostream>
#include "MapFactory.h"
#include "Game.h"
#include "GameControl.h"
#include "GameLogic.h"
#include "math.h"
#include <sstream>     
#include <string>      
#include <vector>      


// Inits new game
void Game::init() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Register mouse callback functions
    glutMouseFunc(GameControl::mouseButton);
    glutMotionFunc(GameControl::mouseMotion);
    glutKeyboardFunc(GameControl::keyboard);

    currentLevel = 0;
    playerLives = 100;

    Game::initLevel();
    gameFont.init("assets/fonts/Roboto-Regular.ttf", 128);
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
    game.renderScore();

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

void Game::renderScore() {
    Game& game = Game::getInstance();
    Map* map = game.getMap();
    Tile* tile = map->getTileAt(MapFactory::MAP_HEIGHT - 1, 1); // Get the target tile

    Point3D textOrigin = tile->getOrigin(); // Get the 3D position of the tile

    std::string scoreText = "Score: " + std::to_string(100);

    // Call print3D with the text, the 3D position (adjusted for your needs),
    // and a world scale for the font size
    game.print3D(game.gameFont, textOrigin.x, textOrigin.y, textOrigin.z, scoreText, 0.001f);
}

void Game::print3D(const glfreetype::font_data& ft_font,
    float x, float y, float z,
    const std::string& text,
    float scale)
{
    // split into lines
    std::vector<std::string> lines;
    {
        std::stringstream ss(text);
        std::string line;
        while (std::getline(ss, line)) lines.push_back(line);
    }

    glPushAttrib(GL_LIST_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // 1) position in world
    glTranslatef(x, y, z);

    // use pixel zoom to shrink bitmaps
    glScalef(scale, scale, scale);

    // common state
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);

    glListBase(ft_font.list_base);

    // Render each line of text
    for (size_t i = 0; i < lines.size(); ++i) {
        glCallLists(lines[i].length(),
            GL_UNSIGNED_BYTE,
            lines[i].c_str());

        // Move down for each line. If you want line spacing, you could adjust this, but now it's simple.
        glTranslatef(0, -1.0f, 0); // Adjust this for space between lines
    }

    glPixelZoom(1.0f, 1.0f);

    glPopMatrix();
    glPopAttrib();
}
