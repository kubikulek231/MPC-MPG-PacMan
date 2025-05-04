#include "GameSounds.h"
#include "Macro.h"
#include "SDL3/SDL_mixer.h"

void GameSounds::init() {
    if (isInit) {
        return;
    }

    if ((Mix_Init(0) & 0) == 0) {
    }

    if (!Mix_OpenAudio(0, nullptr)) {
        SDL_Log("Mix_OpenAudio failed: %s", SDL_GetError());
        return;
    }

    soundBeginning = Mix_LoadWAV(pathBeginning);
    if (!soundBeginning) { SDL_Log("Mix_LoadWAV(\"%s\") failed: %s", SDL_GetError()); }
    soundChomp = Mix_LoadWAV(pathChomp);
    if (!soundChomp) { SDL_Log("Mix_LoadWAV(\"%s\") failed: %s", SDL_GetError()); }
    soundDeath = Mix_LoadWAV(pathDeath);
    if (!soundDeath) { SDL_Log("Mix_LoadWAV(\"%s\") failed: %s", SDL_GetError()); }
    
    // Reserve 3 channgels (0 - 2)
    Mix_ReserveChannels(2);

    isInit = true;
}

void GameSounds::playBeginning() {
	ASSERT_MSG(isInit, "GameSounds::init must be called first!");
	if (!Mix_Playing(channelBeginning)) {
		Mix_PlayChannel(channelBeginning, soundBeginning, 0);
	}
}

void GameSounds::startChomp() {
    playingChomp = true;
}

void GameSounds::playChomp() {
    if (!playingChomp) return;
    if (!Mix_Playing(channelChomp)) {
        Mix_PlayChannel(channelChomp, soundChomp, 0);
    }
}

void GameSounds::stopChomp() {
    playingChomp = false;
}

void GameSounds::playDeath() {
	ASSERT_MSG(isInit, "GameSounds::init must be called first!");
	if (!Mix_Playing(channelDeath)) {
		Mix_PlayChannel(channelDeath, soundDeath, 0);
	}
}