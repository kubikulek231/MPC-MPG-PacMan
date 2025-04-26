#include "GameControl.h"


void GameControl::update() {
    handleWasdMovement();
}

void GameControl::handleWasdMovement() {
    GameUserInput& gi = GameUserInput::getInstance();
    // WASD movement
    if (gi.isKeyFlagPressed('w')) { gi.setMovementChanged(); gi.setMoveDir(MoveDir::FWD); }
    if (gi.isKeyFlagPressed('s')) { gi.setMovementChanged(); gi.setMoveDir(MoveDir::BWD); }
    if (gi.isKeyFlagPressed('a')) { gi.setMovementChanged(); gi.setMoveDir(MoveDir::LEFT); }
    if (gi.isKeyFlagPressed('d')) { gi.setMovementChanged(); gi.setMoveDir(MoveDir::RIGHT); }
}