// FadeTimer.h
#ifndef FADE_TIMER_H
#define FADE_TIMER_H

#include <chrono>

/**
 * FadeTimer drives a fade-in, hold, fade-out cycle for UI text or sprites.
 * You call start(), then repeatedly update(), and query alpha [0..1] via getAlpha().
 */
class FadeTimer {
public:
    enum class Phase {
        In,    // fading in
        Hold,  // fully visible
        Out,   // fading out
        Idle   // not active
    };

    FadeTimer();

    /**
     * Configure durations (in milliseconds) for each phase.
     * @param fadeInMs    time to go from alpha 0 -> 1
     * @param holdMs      time to hold at alpha 1
     * @param fadeOutMs   time to go from alpha 1 -> 0
     */
    void configure(uint64_t fadeInMs, uint64_t holdMs, uint64_t fadeOutMs);

    /**
     * Start the fade cycle now.
     */
    void start();

    /**
     * Advance internal timer; should be called each frame with current time point.
     * @param now  current steady_clock time
     */
    void update(std::chrono::steady_clock::time_point now);

    /**
     * Get current alpha [0.0, 1.0] based on the phase and elapsed time.
     */
    float getAlpha() const;

    /**
     * Get the current phase.
     */
    Phase getPhase() const;

private:
    uint64_t fadeInDuration;
    uint64_t holdDuration;
    uint64_t fadeOutDuration;
    std::chrono::steady_clock::time_point startTime;
    Phase currentPhase;
};

#endif // FADE_TIMER_H