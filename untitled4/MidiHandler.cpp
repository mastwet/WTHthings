#include "MidiHandler.h"
#include <iostream>

MidiHandler::MidiHandler(Parameters* sharedParams)
        : midiStream(nullptr), running(false), params(sharedParams) {}

MidiHandler::~MidiHandler() {
    stop();
    if (midiStream) {
        Pm_Close(midiStream);
    }
    Pm_Terminate();
}

bool MidiHandler::initialize() {
    PmError err = Pm_Initialize();
    if (err != pmNoError) {
        std::cerr << "PortMidi initialization failed: " << Pm_GetErrorText(err) << std::endl;
        return false;
    }

    int numDevices = Pm_CountDevices();
    if (numDevices == 0) {
        std::cerr << "No MIDI devices found." << std::endl;
        return false;
    }

    // Open the first available MIDI input device
    PmDeviceID inputDevice = Pm_GetDefaultInputDeviceID();
    if (inputDevice == pmNoDevice) {
        std::cerr << "No default MIDI input device." << std::endl;
        return false;
    }

    err = Pm_OpenInput(&midiStream, inputDevice, NULL, 512, NULL, NULL);
    if (err != pmNoError) {
        std::cerr << "Failed to open MIDI input: " << Pm_GetErrorText(err) << std::endl;
        return false;
    }

    return true;
}

void MidiHandler::setCallback(MidiCallback cb) {
    callback = cb;
}

void MidiHandler::start() {
    if (!midiStream) return;
    running = true;
    midiThread = std::thread(&MidiHandler::midiLoop, this);
}

void MidiHandler::stop() {
    if (running) {
        running = false;
        if (midiThread.joinable()) {
            midiThread.join();
        }
    }
}

void MidiHandler::midiLoop() {
    while (running) {
        int numEvents = Pm_Poll(midiStream);
        if (numEvents) {
            PmEvent buffer[10];
            int n = Pm_Read(midiStream, buffer, 10);
            if (n > 0 && callback) {
                for(int i = 0; i < n; ++i) {
                    callback(buffer[i]);
                }
            }
        }
        Pt_Sleep(10); // Sleep to prevent high CPU usage
    }
}
