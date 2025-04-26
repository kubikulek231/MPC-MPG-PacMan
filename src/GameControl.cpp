#include "GameControl.h"

void GameControl::update() {
    handleWasdMovement();
    changeCameraMode();
}

void GameControl::handleWasdMovement() {
    GameUserInput& gi = GameUserInput::getInstance();
    // WASD movement
    if (gi.isKeyFlagPressed('w')) { gi.setMovementChanged(); gi.setMoveDir(MoveDir::FWD); }
    if (gi.isKeyFlagPressed('s')) { gi.setMovementChanged(); gi.setMoveDir(MoveDir::BWD); }
    if (gi.isKeyFlagPressed('a')) { gi.setMovementChanged(); gi.setMoveDir(MoveDir::LEFT); }
    if (gi.isKeyFlagPressed('d')) { gi.setMovementChanged(); gi.setMoveDir(MoveDir::RIGHT); }
}

void GameControl::changeCameraMode() {
    Game& game = Game::getInstance();
    GameUserInput& gi = GameUserInput::getInstance();
    GameCamera& gc = GameCamera::getInstance();
    if (gi.isKeyFlagPressedAndReleased('c')) { 
        gi.resetKeyFlagPressedAndReleased('c');
        game.replenishCameraHintFadeTimer();
        gc.setNextCameraMode(); 
    }
}