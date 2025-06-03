#include <MIDI.h>

// Create a MIDI instance
MIDI_CREATE_DEFAULT_INSTANCE();

// Constants
const int buttonPins[] = {2, 3, 4, 5}; // Kick, Snare, Clap, Hi-Hat
const int numButtons = sizeof(buttonPins) / sizeof(buttonPins[0]);
const int debounceTime = 50;

// MIDI notes for each drum sound
const byte drumNotes[] = {36, 38, 39, 42}; // Kick, Snare, Clap, Hi-Hat

// Variables
bool buttonStates[numButtons] = {false};

void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI); // Listen to all incoming messages
}

void loop() {
  for (int i = 0; i < numButtons; i++) {
    bool currentState = digitalRead(buttonPins[i]) == LOW;
    if (currentState != buttonStates[i] && currentState) { // Button is pressed
      MIDI.sendNoteOn(drumNotes[i], 127, 1); // Send MIDI note on message
      delay(debounceTime);
    } else if (currentState != buttonStates[i] && !currentState) { // Button is released
      MIDI.sendNoteOff(drumNotes[i], 0, 1); // Send MIDI note off message
    }
    buttonStates[i] = currentState;
  }
}