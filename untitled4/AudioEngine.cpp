#include "AudioEngine.h"
#include <iostream>

// Constructor
AudioEngine::AudioEngine(Parameters* sharedParams) : stream(nullptr) {
    data.params = sharedParams;
}

// Destructor
AudioEngine::~AudioEngine() {
    stop();
    terminate();
}

bool AudioEngine::initialize() {
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio initialization failed: " << Pa_GetErrorText(err) << std::endl;
        return false;
    }

    int sample_rate = 44100; // Or make this configurable
    data.dsp.init(sample_rate); // Initialize DSP with sample rate
    data.dsp.instanceInit(sample_rate);
    data.dsp.instanceClear();

    return true;
}

bool AudioEngine::start() {
    PaError err = Pa_OpenDefaultStream(&stream,
                                       0,          // no input channels
                                       2,          // stereo output
                                       paFloat32,  // 32 bit floating point output
                                       44100,
                                       256,        // frames per buffer
                                       audioCallback,
                                       &data);
    if (err != paNoError) {
        std::cerr << "Failed to open PortAudio stream: " << Pa_GetErrorText(err) << std::endl;
        return false;
    }

    err = Pa_StartStream(stream);
    if (err != paNoError) {
        std::cerr << "Failed to start PortAudio stream: " << Pa_GetErrorText(err) << std::endl;
        return false;
    }

    data.isRunning = true;
    return true;
}

void AudioEngine::stop() {
    if (stream && data.isRunning) {
        Pa_StopStream(stream);
        Pa_CloseStream(stream);
        data.isRunning = false;
    }
}

void AudioEngine::terminate() {
    Pa_Terminate();
}

int AudioEngine::audioCallback(const void *inputBuffer, void *outputBuffer,
                               unsigned long framesPerBuffer,
                               const PaStreamCallbackTimeInfo* timeInfo,
                               PaStreamCallbackFlags statusFlags,
                               void *userData) {
    auto *data = static_cast<AudioData*>(userData);
    auto *out = static_cast<float*>(outputBuffer);

    // Read parameters atomically
    float currentGain = data->params->gain.load();
    float currentCutoff = data->params->cutoffFrequency.load();
    float currentQ = data->params->qFactor.load();
    bool currentGate = data->params->gate.load();

    // Update DSP parameters if they've changed
    data->dsp.setGain(currentGain);
    data->dsp.setCutoffFrequency(currentCutoff);
    data->dsp.setQFactor(currentQ);
    if (currentGate) {
        data->dsp.toggleGate();
        // Reset gate to false if it's a momentary switch
        data->params->gate.store(false);
    }

    // Prepare DSP output buffer
    FAUSTFLOAT* dspOut = data->dspOutput.data();
    FAUSTFLOAT* outputs[1] = { dspOut };

    // Call DSP compute (no inputs, one output)
    data->dsp.compute(framesPerBuffer, nullptr, outputs);

    // Copy DSP output to both left and right channels
    for(unsigned int i = 0; i < framesPerBuffer; i++) {
        auto sample = static_cast<float>(dspOut[i]);
        *out++ = sample; // Left channel
        *out++ = sample; // Right channel
    }

    return paContinue;
}
