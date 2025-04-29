#include "GameControl.h"

void GameControl::update() {
    handleWasdMovementEnable();
    handleWasdMovement();
    changeCameraMode();
}

void GameControl::enableWasdAfterFullPressCycle() {
    isWasdMovementEnabled = false;
    if (isAnyWasdKeyPressed()) { 
        waitForKeyReleaseFirst = true; 
    }
}

void GameControl::handleWasdMovement() {
    if (!isWasdMovementEnabled) { return; }
    GameUserInput& gi = GameUserInput::getInstance();
    // WASD movement
    if (gi.isKeyFlagPressed('w')) { gi.resetKeyFlagPressed('w'); movementChanged = true; moveDir = MoveDir::FWD; }
    if (gi.isKeyFlagPressed('s')) { gi.resetKeyFlagPressed('s'); movementChanged = true; moveDir = MoveDir::BWD; }
    if (gi.isKeyFlagPressed('a')) { gi.resetKeyFlagPressed('a'); movementChanged = true; moveDir = MoveDir::LEFT; }
    if (gi.isKeyFlagPressed('d')) { gi.resetKeyFlagPressed('d'); movementChanged = true; moveDir = MoveDir::RIGHT; }
}

// Enable WASD movement when any WASD key is pressed and released for the first time
void GameControl::handleWasdMovementEnable() {
    if (isWasdMovementEnabled) { return; }
    GameUserInput& gi = GameUserInput::getInstance();

    if (waitForKeyReleaseFirst) {
        bool isAnyPressed = isAnyWasdKeyPressed();
        // If key is still pressed, reset the flags and return (try next time)
        if (isAnyPressed) {
            gi.resetKeyFlagPressed('d');
            gi.resetKeyFlagPressed('d');
            gi.resetKeyFlagPressed('d');
            gi.resetKeyFlagPressed('d');
            return;
        }
        if (!isAnyPressed) { waitForKeyReleaseFirst = false; };
    }

    if (gi.isKeyFlagPressedAndReleased('w')) { gi.resetKeyFlagPressedAndReleased('w'); isWasdMovementEnabled = true; moveDir = MoveDir::FWD; }
    if (gi.isKeyFlagPressedAndReleased('s')) { gi.resetKeyFlagPressedAndReleased('s'); isWasdMovementEnabled = true; moveDir = MoveDir::BWD; }
    if (gi.isKeyFlagPressedAndReleased('a')) { gi.resetKeyFlagPressedAndReleased('a'); isWasdMovementEnabled = true; moveDir = MoveDir::LEFT; }
    if (gi.isKeyFlagPressedAndReleased('d')) { gi.resetKeyFlagPressedAndReleased('d'); isWasdMovementEnabled = true; moveDir = MoveDir::RIGHT; }
}

bool GameControl::isAnyWasdKeyPressed() {
    GameUserInput& gi = GameUserInput::getInstance();
    if (gi.isKeyPressed('w')) { return true; }
    if (gi.isKeyPressed('s')) { return true; }
    if (gi.isKeyPressed('a')) { return true; }
    if (gi.isKeyPressed('d')) { return true; }
    return false;
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