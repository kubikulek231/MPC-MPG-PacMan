#ifndef GAMESOUNDS_H
#define GAMESOUNDS_H

#include "gl_includes.h"
#include <string>
#include <vector>
#include <SDL3/SDL.h>
#include <SDL3/SDL_mixer.h>

static class GameSounds {
public:
    static GameSounds& getInstance() {
        static GameSounds instance;
        return instance;
    }
    
    void init();
    void playBeginning();
    void startChomp();
    void stopChomp();
    void playChomp();
    void playDeath();
private:
    bool isInit = false;

    int channelBeginning = 0;
    int channelChomp = 1;
    int channelDeath = 2;

    bool stoppingChomp = false;
    bool playingChomp = false;

    const char* pathBeginning = "assets/sounds/pacman_beginning.wav";
    const char* pathDeath = "assets/sounds/pacman_death.wav";
    const char* pathChomp = "assets/sounds/pacman_chomp.wav";

    Mix_Chunk* soundBeginning = nullptr;
    Mix_Chunk* soundDeath = nullptr;
    Mix_Chunk* soundChomp = nullptr;
};

#endif
