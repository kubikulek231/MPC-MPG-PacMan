#include "Player.h"
#include <GL/glut.h>
#include <chrono>
#include <iostream>
#include "Pi.h"
#include "GameLighting.h"
#include "GameSounds.h"

using namespace std::chrono;

Player::Player() {
}

Player::Player(const Player& other) {
    this->boundingBox = other.boundingBox;
    this->origin = other.origin;

    this->blinkDurationMs = DEFAULT_BLINK_DURATION_MS;
    this->invincibleEndTimeAfterMs = DEFAULT_INVINCIBLE_EXPIRE_AFTER_MS;
}

Player::Player(Map* map, Point3D playerOrigin, BoundingBox3D playerBoundingBox) 
: MovableEntity(map,
                playerOrigin,
                playerBoundingBox,
                Player::DEFAULT_MOVE_DIR,
                Player::DEFAULT_SPEED,
                Player::DEFAULT_SNAP_DISTANCE,
                Player::DEFAULT_DIR_CHANGE_REQUEST_EXPIRE,
                Player::DEFAULT_DIR_CHANGE_REQUEST_EXPIRE_AFTER_MS) {

    this->lastOrigin = this->origin;
    this->blinkDurationMs = DEFAULT_BLINK_DURATION_MS;
    this->invincibleEndTimeAfterMs = DEFAULT_INVINCIBLE_EXPIRE_AFTER_MS;
}

void Player::render() {
    Point3D c = getAbsoluteCenterPoint();
    glPushMatrix();
        glTranslatef(c.x, c.y + 0.25f, c.z);

        // Rotate to face movement direction
        glRotatef(getMoveDirRotationAngle(), 0.0f, 1.0f, 0.0f);

        // Rotate so Pac-Man faces forward
        glRotatef(90.0f, 0.0f, 0.0f, 1.0f);

        float scale = 1.0f;
        float mouthDeg = 30.0f * playerMouthAnimationState;
        if (playerDeathAnimating) {
            scale = 1.0f - playerDeathAnimationState;
        }
        glScalef(scale, scale, scale);

        // Prepare lighting material (instead of glColor)
        GLfloat bodyAmbient[] = { playerBodyColorRed * 0.2f, playerBodyColorGreen * 0.2f, playerBodyColorBlue * 0.2f, 1.0f };
        GLfloat bodyDiffuse[] = { playerBodyColorRed * 0.5f, playerBodyColorGreen * 0.5f, playerBodyColorBlue * 0.5f, 1.0f };
        GLfloat bodySpecular[] = { 0.4f, 0.4f, 0.4f, 1.0f };
        GLfloat bodyEmission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        GLfloat shininess = 32.0f;

        GameLighting::setMaterial(GL_FRONT_AND_BACK, bodyAmbient, bodyDiffuse, bodySpecular, bodyEmission, shininess);

        float R = 0.75f;

        // Compute clipping angles
        float invDeg = 180.0f - 60.0f - mouthDeg;

        if (playerDeathAnimating) {
            float fullyOpen = 150.f;
            invDeg = invDeg + fullyOpen * playerDeathAnimationState;
        }

        float half = invDeg * (PI / 180.0f);

        GLdouble eq0[4] = { +sin(half), 0.0, -cos(half), 0.0 };
        GLdouble eq1[4] = { -sin(half), 0.0, -cos(half), 0.0 };

        if (!playerDeathAnimating || invDeg <= 180.0f) {
            // Draw two spheres separately for mouth
            glPushMatrix();
                glClipPlane(GL_CLIP_PLANE0, eq0);  glEnable(GL_CLIP_PLANE0);
                glutSolidSphere(R, 32, 32);
                glDisable(GL_CLIP_PLANE0);
            glPopMatrix();

            glPushMatrix();
                glClipPlane(GL_CLIP_PLANE1, eq1);  glEnable(GL_CLIP_PLANE1);
                glutSolidSphere(R, 32, 32);
                glDisable(GL_CLIP_PLANE1);
            glPopMatrix();
        }

        if (playerDeathAnimating && invDeg > 180.0f) {
            // Draw only the union if past 180 degrees
            glPushMatrix();
                glClipPlane(GL_CLIP_PLANE0, eq0);
                glEnable(GL_CLIP_PLANE0);

                glClipPlane(GL_CLIP_PLANE1, eq1);
                glEnable(GL_CLIP_PLANE1);

                glutSolidSphere(R, 32, 32);

                glDisable(GL_CLIP_PLANE0);
                glDisable(GL_CLIP_PLANE1);
            glPopMatrix();
        }

        GameLighting::resetMaterial(GL_FRONT_AND_BACK);

        // If dying, skip eyes etc
        if (playerDeathAnimating) {
            glPopMatrix();
            return;
        }

        // Disable lighting for this part
        glDisable(GL_LIGHTING);
        
            // --- INNER MOUTH ---
            glColor3f(0.6f, 0.3f, 0.0f);
            // Render the inner top
            glPushMatrix();
                glClipPlane(GL_CLIP_PLANE0, eq0);
                glEnable(GL_CLIP_PLANE0);
                glutSolidSphere(0.75f, 32, 32);
                glDisable(GL_CLIP_PLANE0);
            glPopMatrix();

            // Render the inner bot
            glPushMatrix();
                glClipPlane(GL_CLIP_PLANE1, eq1);
                glEnable(GL_CLIP_PLANE1);
                glutSolidSphere(0.75f, 32, 32);
                glDisable(GL_CLIP_PLANE1);
            glPopMatrix();

            // --- FILL MOUTH GAP WITH DISKS ---
            GLUquadric* disk = gluNewQuadric();
            glDisable(GL_CULL_FACE);
            glPushMatrix();
                glRotatef(invDeg, 0, 1, 0);
                gluDisk(disk, 0.0, 0.75, 32, 1);
            glPopMatrix();
            glPushMatrix();
                glRotatef(-invDeg, 0, 1, 0);
                gluDisk(disk, 0.0, 0.75, 32, 1);
            glPopMatrix();
            gluDeleteQuadric(disk);

        glEnable(GL_LIGHTING);

        // --- EYES & PUPILS ---
        // Eye
        GLfloat eyeAmbient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
        GLfloat eyeDiffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };  // White for the eyes
        GLfloat eyeSpecular[4] = { 0.9f, 0.9f, 0.9f, 1.0f }; // Shiny eyes
        GLfloat eyeEmission[4] = { 0.0f, 0.0f, 0.0f, 1.0f };  // No emission
        GLfloat eyeShininess = 128.0f;

        auto drawEye = [](float tx, float ty, float tz, float rz, float ry) {
            glPushMatrix();
                glColor3f(1.0f, 1.0f, 1.0f);
                glTranslatef(tx, ty, tz);
                glRotatef(rz, 0.0f, 0.0f, 1.0f);
                glRotatef(ry, 0.0f, 1.0f, 0.0f);
                glScalef(1.0f, 1.0f, 0.3f);
                glutSolidSphere(0.20f, 12, 12);
            glPopMatrix();
            };

        // Pupil
        GLfloat pupilAmbient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };  // Black ambient
        GLfloat pupilDiffuse[4] = { 0.0f, 0.0f, 0.0f, 1.0f };  // Black diffuse
        GLfloat pupilSpecular[4] = { 0.2f, 0.2f, 0.2f, 1.0f }; // Slightly shiny (small specular)
        GLfloat pupilEmission[4] = { 0.0f, 0.0f, 0.0f, 1.0f };  // No emission
        GLfloat pupilShininess = 10.0f;  // Low shininess

        auto drawPupil = [](float tx, float ty, float tz, float rz, float ry) {
            glPushMatrix();
                glTranslatef(tx, ty, tz);
                glRotatef(rz, 0.0f, 0.0f, 1.0f);
                glRotatef(ry, 0.0f, 1.0f, 0.0f);
                glScalef(1.0f, 1.0f, 0.3f);
                glutSolidSphere(0.11f, 12, 12);
            glPopMatrix();
            };

        GameLighting::setMaterial(GL_FRONT_AND_BACK, eyeAmbient, eyeDiffuse, eyeSpecular, eyeEmission, eyeShininess);
            drawEye(0.54f, 0.27f, -0.41f, 35.0f, -55.0f); // Left eye
            drawEye(0.54f, -0.27f, -0.41f, 145.0f, -125.0f); // Right eye
        GameLighting::resetMaterial(GL_FRONT_AND_BACK);

        GameLighting::setMaterial(GL_FRONT_AND_BACK, pupilAmbient, pupilDiffuse, pupilSpecular, pupilEmission, pupilShininess);
            drawPupil(0.535f, 0.27f, -0.50f, 40.0f, -44.5f); // Left pupil
            drawPupil(0.535f, -0.27f, -0.50f, 140.0f, -135.5f); // Right pupil
        GameLighting::resetMaterial(GL_FRONT_AND_BACK);
    glPopMatrix();
}


void Player::move(MoveDir requestedMoveDir, bool& isNewRequest, float frameTimeMs) {
    // Handle movement and teleportation
    this->MovableEntity::move(requestedMoveDir, isNewRequest, frameTimeMs);
    this->teleport(moveDir);
    
    if (moveDir == MoveDir::UNDEFINED || moveDir == MoveDir::NONE) { return; }

    auto tiles = intersectingTiles(this);
    Tile* tileCurrent = !tiles.empty() ? currentTile(tiles) : nullptr;
    Tile* nextTileInDir = tileCurrent != nullptr ? nextTileInDirection(moveDir, tileCurrent) : nullptr;
    Tile* nextNextTileInDir = tileCurrent != nullptr ? nextTileInDir->getTileInMoveDir(moveDir) : nullptr;
    bool isPelletNext = nextTileInDir != nullptr && nextTileInDir->getTileType() == TileType::PELLET;
    bool isPelletNextNext = nextNextTileInDir != nullptr && nextNextTileInDir->getTileType() == TileType::PELLET;
    updateMouthAnimation(frameTimeMs, isPelletNext);

    if (isPelletNext || isPelletNextNext) {
        GameSounds::getInstance().startChomp();
    }
    else {
        GameSounds::getInstance().stopChomp();
    }
    GameSounds::getInstance().GameSounds::playChomp();

    //float newDistance = this->origin.distanceTo2D(this->lastOrigin);
    //if (speedoMeter.update(newDistance, 20)) { std::cout << "Move distance: " << speedoMeter.getAverage() << std::endl; }
    //this->lastOrigin = this->origin;
}

void Player::update(int& totalCollectedPellets) {
    uint64_t currentTimeMs = getTimeMs();

    // Collect pellets
    auto tiles = this->intersectingTiles(this);
    auto tile = currentTile(tiles);
    if (tile && map->collectPellet(tile)) {
        totalCollectedPellets++;
    }

    // Update invincibility and blinking
    if (getIsInvincible()) {
        if (currentTimeMs > invincibleEndTimeMs) { isInvincible = false; }
        if (currentTimeMs > nextBlinkTimeMs) {
            invincibleBlink = !invincibleBlink;
            nextBlinkTimeMs = currentTimeMs + blinkDurationMs;
        }
        if (invincibleBlink) { playerBodyColorBlue = 1.0f; }
        else { playerBodyColorBlue = 0.0f; }
    }
}

void Player::updateMouthAnimation(float frameTimeMs, bool keepAnimating) {
    if (!playerMouthOpening && !playerMouthClosing)
        return;

    // dt in seconds
    float dt = frameTimeMs;

    float baseSpeed = speed * 2.0f;            // e.g. speed = 1.0f means 1.0 per second

    const float minFactor = 0.3f;       // never go slower than 30%
    float f = sinf(PI * playerMouthAnimationState);
    float easeFactor = minFactor + (1.0f - minFactor) * f;

    // final delta
    float dstate = baseSpeed * dt * easeFactor;

    // apply to opening / closing
    if (playerMouthOpening) {
        playerMouthAnimationState = std::min(playerMouthAnimationState + dstate, 1.0f);
        if (playerMouthAnimationState >= 1.0f) {
            playerMouthOpening = false;
            playerMouthClosing = true;
        }
        return;
    }

    // closing
    playerMouthAnimationState = std::max(playerMouthAnimationState - dstate, 0.0f);
    if (playerMouthAnimationState <= 0.0f && keepAnimating) {
        playerMouthOpening = true;
        playerMouthClosing = false;
    }
}

bool Player::updateDeathAnimation(float frameTimeMs) {
    if (!playerDeathAnimating) return true;

    float dt = frameTimeMs / 1.4f;
    const float duration = 1.0f;

    // Advance the timer
    playerDeathAnimationState += dt / duration;

    if (playerDeathAnimationState >= 1.0f) {
        playerDeathAnimationState = 1.0f;
        playerDeathAnimating = false;
        return true;
    }
    return false;
}

void Player::setIsInvincible() {
    uint64_t currentTime = getTimeMs();
    invincibleEndTimeMs = currentTime + invincibleEndTimeAfterMs;
    isInvincible = true;
}

uint64_t Player::getTimeMs() {
    return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

void Player::renderDeathAnimation() {
    float deathMouth = 180.0f;
    float deathScale = 1.0f - playerDeathAnimationState;

    glPushMatrix();
        // scale down the whole mesh
        glScalef(deathScale, deathScale, deathScale);

        // position/orientation same as alive
        Point3D c = getAbsoluteCenterPoint();
        glTranslatef(c.x, c.y + 0.25f, c.z);
        glRotatef(getMoveDirRotationAngle(), 0, 1, 0);
        glRotatef(90, 0, 0, 1);

        // Now clip Pac-Man open by deathMouth on each side
        float halfRad = (180.0f - deathMouth) * (PI / 180.0f) * 0.5f;
        GLdouble eq0[4] = { +sin(halfRad), 0.0, -cos(halfRad), 0.0 };
        GLdouble eq1[4] = { -sin(halfRad), 0.0, -cos(halfRad), 0.0 };
        // render clipped sphere once, no inner mouth/fill
        //glEnable(GL_CLIP_PLANE0);
        //glClipPlane(GL_CLIP_PLANE0, eq0);
        //glEnable(GL_CLIP_PLANE1);
        //glClipPlane(GL_CLIP_PLANE1, eq1);

        // draw the body
        glutSolidSphere(0.75f, 32, 32);

        //glDisable(GL_CLIP_PLANE0);
        //glDisable(GL_CLIP_PLANE1);
    glPopMatrix();
    return;   // done
}