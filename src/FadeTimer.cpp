// FadeTimer.cpp
#include "FadeTimer.h"

FadeTimer::FadeTimer()
    : fadeInDuration(500)
    , holdDuration(1000)
    , fadeOutDuration(500)
    , startTime()
    , currentPhase(Phase::Idle)
{
}

void FadeTimer::configure(uint64_t fadeInMs, uint64_t holdMs, uint64_t fadeOutMs) {
    fadeInDuration = fadeInMs;
    holdDuration = holdMs;
    fadeOutDuration = fadeOutMs;
}

void FadeTimer::start() {
    startTime = std::chrono::steady_clock::now();
    currentPhase = Phase::In;
}

void FadeTimer::update(std::chrono::steady_clock::time_point now) {
    if (currentPhase == Phase::Idle) return;

    uint64_t elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();
    if (elapsed < fadeInDuration) {
        currentPhase = Phase::In;
    }
    else if (elapsed < fadeInDuration + holdDuration) {
        currentPhase = Phase::Hold;
    }
    else if (elapsed < fadeInDuration + holdDuration + fadeOutDuration) {
        currentPhase = Phase::Out;
    }
    else {
        currentPhase = Phase::Idle;
    }
}

float FadeTimer::getAlpha() const {
    if (currentPhase == Phase::Idle) return 0.0f;
    uint64_t t = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - startTime).count();

    if (currentPhase == Phase::In) {
        return static_cast<float>(t) / fadeInDuration;
    }
    else if (currentPhase == Phase::Hold) {
        return 1.0f;
    }
    else if (currentPhase == Phase::Out) {
        uint64_t tOut = t - (fadeInDuration + holdDuration);
        return 1.0f - static_cast<float>(tOut) / fadeOutDuration;
    }
    return 0.0f;
}

FadeTimer::Phase FadeTimer::getPhase() const {
    return currentPhase;
}