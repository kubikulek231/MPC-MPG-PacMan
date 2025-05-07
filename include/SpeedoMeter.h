#include <vector>
#include <numeric>

#ifndef SPEEDOMETER_H
#define SPEEDOMETER_H

// Class for debugging MovableEntity speed :)
class SpeedoMeter {
private:
    int counter = 0;
    int numSamples;
    std::vector<float> samples;

public:
    SpeedoMeter(int numSamples) : numSamples(numSamples) {}

    bool update(float moveDistance, int counterToTrue) {
        // Maintain a rolling buffer of size numSamples
        if (samples.size() >= numSamples) {
            samples.erase(samples.begin());  // remove oldest
        }
        samples.push_back(moveDistance);

        counter++;
        if (counter >= counterToTrue) {
            counter = 0;
            return true;
        }
        return false;
    }

    float getAverage() {
        if (samples.empty()) return 0.0f;
        float sum = std::accumulate(samples.begin(), samples.end(), 0.0f);
        return sum / samples.size();
    }
};

#endif