#include "AudioEngine.h"
#include "MidiHandler.h"
#include "SynthGUI.h"
#include <iostream>
#include <memory>
#include <thread>

int main(int argc, char **argv) {
    // Create shared parameters
    Parameters sharedParams;

    // Initialize Audio Engine with shared parameters
    AudioEngine audio(&sharedParams);
    if (!audio.initialize()) {
        std::cerr << "Audio Initialization Failed." << std::endl;
        return -1;
    }

    // Initialize MIDI Handler with shared parameters
    MidiHandler midi(&sharedParams);
    bool midiInitialized = false;
    if (midi.initialize()) {
        midiInitialized = true;
    } else {
        std::cerr << "No MIDI device found." << std::endl;
    }

    // Initialize SynthGUI with shared parameters
    SynthGUI gui(&sharedParams);
    gui.create();

    // Set MIDI callback if MIDI is initialized
    if (midiInitialized) {
        midi.setCallback([&gui, &sharedParams](PmEvent event) {
            // Process MIDI event and update GUI accordingly
            unsigned char status = Pm_MessageStatus(event.message);
            unsigned char data1 = Pm_MessageData1(event.message);
            unsigned char data2 = Pm_MessageData2(event.message);
            std::string msg = "MIDI Event: " + std::to_string(status) + " " +
                              std::to_string(data1) + " " +
                              std::to_string(data2);
            gui.displayMessage(msg);

            // Map MIDI events to DSP parameter changes
            if ((status & 0xF0) == 0xB0) { // Control Change
                if (data1 == 7) { // Volume CC
                    float newGain = data2 / 127.0f;
                    sharedParams.gain.store(newGain);
                    gui.displayMessage("MIDI CC7: Setting Gain to " + std::to_string(newGain));
                }
                // Add more mappings as needed
            } else if ((status & 0xF0) == 0x90) { // Note On
                if (data2 > 0) { // Note On with velocity
                    sharedParams.gate.store(true);
                    gui.displayMessage("MIDI Note On: Gate On");
                }
            } else if ((status & 0xF0) == 0x80 || ((status & 0xF0) == 0x90 && data2 == 0)) { // Note Off
                sharedParams.gate.store(false);
                gui.displayMessage("MIDI Note Off: Gate Off");
            }
        });
        midi.start();
    }

    // Start audio stream
    if (!audio.start()) {
        std::cerr << "Failed to start Audio Engine." << std::endl;
        return -1;
    }

    // Show GUI
    gui.show();

    // Run FLTK's event loop in the main thread
    int result = Fl::run();

    // Cleanup will be handled by destructors
    return result;
}
