#ifndef MIDIHANDLER_H
#define MIDIHANDLER_H

#include <portmidi.h>
#include <porttime.h>
#include <thread>
#include <atomic>
#include <functional>
#include "Parameters.h"

class MidiHandler {
public:
    using MidiCallback = std::function<void(PmEvent)>;

    MidiHandler(Parameters* sharedParams);
    ~MidiHandler();
    bool initialize();
    void setCallback(MidiCallback cb);
    void start();
    void stop();

private:
    void midiLoop();
    PmStream *midiStream;
    std::thread midiThread;
    std::atomic<bool> running;
    MidiCallback callback;
    Parameters* params;
};

#endif // MIDIHANDLER_H
