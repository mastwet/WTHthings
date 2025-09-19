#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H

#include <portaudio.h>
#include <atomic>
#include <vector>
#include "dsp.h"
#include "Parameters.h"

struct AudioData {
    std::atomic<bool> isRunning;
    dspmain dsp; // Instance of your DSP class
    std::vector<FAUSTFLOAT> dspOutput; // Buffer for DSP output
    Parameters* params; // Pointer to shared parameters

    // Constructor to initialize members
    AudioData() : isRunning(false), dspOutput(256, 0.0f), params(nullptr) {}
};

class AudioEngine {
public:
    AudioEngine(Parameters* sharedParams);
    ~AudioEngine();
    bool initialize();
    bool start();
    void stop();
    void terminate();

    AudioData data; // Make AudioData public for access in main.cpp

private:
    static int audioCallback(const void *inputBuffer, void *outputBuffer,
                             unsigned long framesPerBuffer,
                             const PaStreamCallbackTimeInfo* timeInfo,
                             PaStreamCallbackFlags statusFlags,
                             void *userData);
    PaStream *stream;
};

#endif // AUDIOENGINE_H
