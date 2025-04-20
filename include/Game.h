#ifndef GAME_H
#define GAME_H

#include "Map.h"
#include "Player.h"
#include "MapFactory.h"
#include "MoveDir.h"
#include "Ghost.h"

// Singleton class
class Game {
public:
    static Game& getInstance() {
        static Game instance;  // This will ensure the instance is created only once
        return instance;
    }

    static constexpr float LEVEL_SPEED_INCREMENT = 0.5f;
    static constexpr float GHOST_SPEED_COMP = 0.05f;

    void init();    // Init new game along with OpenGL settings
    void initLevel(int level = -1);
    static void update(int value = 0);  // Update game logic
    static void render();  // Display the game scene

    // GLUT Callbacks
    static void reshape(int w, int h);

    // Getters
    Map* getMap() { return &map; }
    Player* getPlayer() { return &player; }
    MoveDir* getMoveDir() { return &moveDir; }
    std::vector<Ghost*>& getGhosts() { return ghosts; }
    float getLastFrameTimeDeltaSeconds() const { return lastFrameTimeDeltaSeconds; }
    float getCameraDistance() const { return cameraDistance; }
    float getCameraAngleX() const { return cameraAngleX; }
    float getCameraAngleY() const { return cameraAngleY; }
    float getCameraPosX() const { return cameraPosX; }
    float getCameraPosZ() const { return cameraPosZ; }
    bool getIsLeftMousePressed() const { return isLeftMousePressed; }
    bool getIsMiddleMousePressed() const { return isMiddleMousePressed; }
    bool& getIsDirectionKeyPressed() { return isDirectionKeyPressed; }
    int getLastMouseX() const { return lastMouseX; }
    int getLastMouseY() const { return lastMouseY; }
    float getBaseSpeed() const { return baseMoveSpeed; }
    int getPlayerLives() const { return playerLives; }
    int getCurrentLevel() const { return currentLevel; }

    // Setters
    void setMap(Map newMap) { map = newMap; }
    void setPlayer(Player newPlayer) { player = newPlayer; }
    void setMoveDir(MoveDir newMoveDir) { moveDir = newMoveDir; }
    void setCameraDistance(float distance) { cameraDistance = distance; }
    void setCameraAngleX(float angleX) { cameraAngleX = angleX; }
    void setCameraAngleY(float angleY) { cameraAngleY = angleY; }
    void setCameraPosX(float posX) { cameraPosX = posX; }
    void setCameraPosZ(float posY) { cameraPosZ = posY; }
    void setIsLeftMousePressed(bool pressed) { isLeftMousePressed = pressed; }
    void setIsMiddleMousePressed(bool pressed) { isMiddleMousePressed = pressed; }
    void setIsDirectionKeyPressed(bool changed) { isDirectionKeyPressed = changed; }
    void setLastMouseX(int x) { lastMouseX = x; }
    void setLastMouseY(int y) { lastMouseY = y; }
    void setBaseSpeed(float speed) { baseMoveSpeed = speed; }
    void setPlayerLives(int lives) { playerLives = lives; }
    void setCurrentLevel(int level) { currentLevel = level; }

    int gameCollectedPellets = 0;
private:
    Game() = default;
    Game(const Game&) = delete;  // Prevent copy constructor
    Game& operator=(const Game&) = delete;  // Prevent assignment operator

    MapFactory mapFactory;
    Map map;
    Player player;
    Ghost pinky;
    Ghost blinky;
    Ghost inky;
    Ghost clyde;
    std::vector<Ghost*> ghosts;
    MoveDir moveDir;
    bool isDirectionKeyPressed = true;
    float lastFrameTimeSeconds = 0.0f;
    float lastFrameTimeDeltaSeconds = 0.0f;
    float cameraDistance = 50.0f;
    float cameraAngleX = 0.0f;  // Rotation around the X-axis (pitch)
    float cameraAngleY = 0.0f;  // Rotation around the Y-axis (yaw)
    float cameraPosX = 0.0f;
    float cameraPosZ = 0.0f;
    bool isLeftMousePressed = false; // To check if the left mouse button is pressed
    bool isMiddleMousePressed = false;
    int lastMouseX, lastMouseY;  // To track the last mouse position
    float baseMoveSpeed = 2.5f;
    const float maxFrametimeNormalizedSpeed = 0.5f;
    int playerLives;
    int currentLevel;
};

#endif // GAME_H
