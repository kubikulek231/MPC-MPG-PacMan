#ifndef GAMELOGIC_H
#define GAMELOGIC_H

class Game; // Forward declaration to avoid circular dependency

enum class MoveDir {
	FWD = 0,
	BWD = 1,
	RIGHT = 2,
	LEFT = 3,
};

class GameLogic {
public:
    static void update(float frameTimeSeconds);
	static EntityPosition getNextPlayerPos(MoveDir moveDir, EntityPosition currentPlayerPos, float speedNormalized);
};

#endif
