#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <atomic>

struct Parameters {
    std::atomic<float> gain;
    std::atomic<float> cutoffFrequency;
    std::atomic<float> qFactor;
    std::atomic<bool> gate;

    Parameters()
            : gain(1.0f),
              cutoffFrequency(500.0f),
              qFactor(5.0f),
              gate(false) {}
};

#endif // PARAMETERS_H
