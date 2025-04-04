#ifndef GAME_H
#define GAME_H

#include "Map.h"
#include "Player.h"
#include "MapFactory.h"

// Singleton class
class Game {
public:
    static Game& getInstance() {
        static Game instance;  // This will ensure the instance is created only once
        return instance;
    }

    void init();    // Initialize OpenGL settings
    static void update(int value = 0);  // Update game logic
    static void render();  // Display the game scene

    // GLUT Callbacks
    static void reshape(int w, int h);

    // Getters
    Map& getMap() { return map; }
    Player& getPlayer() { return player; }
    MoveDir getMoveDir() const { return moveDir; }
    float getLastFrameTime() const { return lastFrameTime; }
    float getCameraDistance() const { return cameraDistance; }
    float getCameraAngleX() const { return cameraAngleX; }
    float getCameraAngleY() const { return cameraAngleY; }
    bool getIsMousePressed() const { return isMousePressed; }
    int getLastMouseX() const { return lastMouseX; }
    int getLastMouseY() const { return lastMouseY; }
    float getMoveSpeed() const { return moveSpeed; }

    // Setters
    void setMap(const Map& newMap) { map = newMap; }
    void setPlayer(const Player& newPlayer) { player = newPlayer; }
    void setMoveDir(MoveDir newMoveDir) { moveDir = newMoveDir; }
    void setLastFrameTime(float time) { lastFrameTime = time; }
    void setCameraDistance(float distance) { cameraDistance = distance; }
    void setCameraAngleX(float angleX) { cameraAngleX = angleX; }
    void setCameraAngleY(float angleY) { cameraAngleY = angleY; }
    void setIsMousePressed(bool pressed) { isMousePressed = pressed; }
    void setLastMouseX(int x) { lastMouseX = x; }
    void setLastMouseY(int y) { lastMouseY = y; }
    void setMoveSpeed(float speed) { moveSpeed = speed; }
private:
    Game() = default;
    Game(const Game&) = delete;  // Prevent copy constructor
    Game& operator=(const Game&) = delete;  // Prevent assignment operator

    MapFactory mapFactory = MapFactory();
    Map map = mapFactory.createMap();
    Player player = Player(0.0f, 0.0f);;
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
