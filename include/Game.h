#ifndef GAME_H
#define GAME_H

#include "gl_includes.h"
#include "Map.h"
#include "Player.h"
#include "MapFactory.h"
#include "MoveDir.h"
#include "Ghost.h"
#include "GameControl.h"
#include "glft2/TextRenderer.hpp"

// Singleton class
class Game {
public:
    static Game& getInstance() {
        static Game instance;  // This will ensure the instance is created only once
        return instance;
    }

    static constexpr float LEVEL_SPEED_INCREMENT = 0.5f;
    static constexpr float GHOST_SPEED_COMP = 0.1f;
    static constexpr float LEVEL_DURATION_MULTIPLIER = 0.88f;

    void init();    // Init new game along with OpenGL settings
    void initLevel(int level = -1);
    static void update(int value = 0);  // Update game logic
    static void render();  // Display the game scene
    static void renderScore();
    static void renderLives();

    // GLUT Callbacks
    static void reshape(int w, int h);

    // Getters
    Map* getMap() { return &map; }
    Player* getPlayer() { return &player; }
    std::vector<Ghost*>& getGhosts() { return ghosts; }
    float getLastFrameTimeDeltaSeconds() const { return lastFrameTimeDeltaS; }
    float getBaseSpeed() const { return baseMoveSpeed; }
    int getPlayerLives() const { return playerLives; }
    int getCurrentLevel() const { return currentLevel; }
    int getTotalScore() const { return totalScore; }

    glft2::font_data getGameFont() const { return gameFont; }
    glft2::font_data getMenuFont() const { return menuFont; }
    
    GameControl& getGameControlInstance() { return gameControl; }


    // Setters
    void setMap(Map newMap) { map = newMap; }
    void setPlayer(Player newPlayer) { player = newPlayer; }
    void setBaseSpeed(float speed) { baseMoveSpeed = speed; }
    void setPlayerLives(int lives) { playerLives = lives; }
    void setCurrentLevel(int level) { currentLevel = level; }
    void setTotalScore(int score) { totalScore = score; }

    int gameCollectedPellets = 0;
private:
    Game() = default;
    Game(const Game&) = delete;  // Prevent copy constructor
    Game& operator=(const Game&) = delete;  // Prevent assignment operator

    GameControl& gameControl = GameControl::getInstance();
    
    MapFactory mapFactory;
    Map map;
    Player player;
    Ghost pinky;
    Ghost blinky;
    Ghost inky;
    Ghost clyde;
    std::vector<Ghost*> ghosts;
    MoveDir moveDir;

    float lastFrameTimeS = 0.0f;
    float lastFrameTimeDeltaS = 0.0f;

    float baseMoveSpeed = 4.5f;
    const float maxFrametimeNormalizedSpeed = 0.5f;
    int playerLives;
    int currentLevel;
    int totalScore;
    glft2::font_data gameFont;
    glft2::font_data menuFont;
};

#endif // GAME_H
