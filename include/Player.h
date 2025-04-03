#ifndef PLAYER_H
#define PLAYER_H

enum class MoveDir {
    FWD = 0,
    BWD = 1,
    RIGHT = 2, 
    LEFT = 3,
};

class Player {
public:
    Player();
    Player(float initPosX, float initPosY);
    static constexpr float DEFAULT_SPEED = 0.01f;
    static constexpr float TILE_SIZE = 1.0f;
    void render();
    float getPosX() const;
    float getPosY() const;
    void move(MoveDir moveDir, float frameTime, float speed = DEFAULT_SPEED);
private:
    void internalMove(MoveDir moveDir, float speed);
    float posX;
    float posY;
};


#endif // PLAYER_H
