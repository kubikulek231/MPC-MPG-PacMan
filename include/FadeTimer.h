// FadeTimer.h
#ifndef FADE_TIMER_H
#define FADE_TIMER_H

#include <chrono>

// FadeTimer drives a fade-in, hold, fade-out cycle for UI text or sprites.
// You call start(), then repeatedly update(), and query alpha [0..1] via getAlpha().
class FadeTimer {
public:
    enum class Phase {
        In,    // fading in
        Hold,  // fully visible
        Out,   // fading out
        Idle   // not active
    };

    FadeTimer();
    void configure(uint64_t fadeInMs, uint64_t holdMs, uint64_t fadeOutMs);
    void start();
    void update(std::chrono::steady_clock::time_point now);
    float getAlpha() const;
    Phase getPhase() const;

private:
    uint64_t fadeInDuration;
    uint64_t holdDuration;
    uint64_t fadeOutDuration;
    std::chrono::steady_clock::time_point startTime;
    Phase currentPhase;
};

#endif // FADE_TIMER_H