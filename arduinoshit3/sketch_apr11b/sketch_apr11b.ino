#include <MIDI.h>

#define SQUENCER_NEXT_NOTE 1
#define SQUENCER_PRE_NOTE 0
#define SQUENCER_WORK_MODE_SET 0
#define SQUENCER_WORK_MODE_PLAY 1

MIDI_CREATE_DEFAULT_INSTANCE();

// Sequencer settings
int sequence[numKeys] = { 0 };
int currentStep = 0;
bool playSequence = false;
unsigned long noteDuration;

char* sequz_note_map[12] = { "C1", "D1B", "D1", "E1B", "E1", "F1", "F1#", "G1", "A1B", "A1", "B1B", "B" };
int sequz_midi_map[12] = { 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139 };
int sequencer_note[12] = {0};

// Initialize MIDI
MIDI.begin(MIDI_CHANNEL_OMNI);

void setup() {
  // Initialize serial
  Serial.begin(9600);
  MIDI.begin(MIDI_CHANNEL_OMNI);
}

void loop() {
}

// read encoder.if encoder left turn, return 1.if encoder right turn, return -1.if encoder not turn, return 0.
int encoder_read() {
  int encoder_value = 0;
  if (digitalRead(encoderPinA) == LOW && digitalRead(encoderPinB) == HIGH) {
    encoder_value = 1;
  } else if (digitalRead(encoderPinA) == HIGH && digitalRead(encoderPinB) == LOW) {
    encoder_value = -1;
  } else {
    encoder_value = 0;
  }
  return encoder_value;
}

void updateBPM() {
  // show bpm on u8g2 display oled12864,encoder input
  int encoder_value = encoder_read();
  if (encoder_value != 0) {
    if (encoder_value == 1) {
      bpm++;
    } else if (encoder_value == -1) {
      bpm--;
    }
    noteDuration = 60000.0 / (bpm * (numKeys / 4.0));  // Calculate the note duration based on BPM and number of steps

    u8g2.clearBuffer();
    u8g2.setCursor(0, 10);
    u8g2.print("BPM:");
    u8g2.setCursor(0, 20);
    u8g2.print(bpm);
    u8g2.sendBuffer();
  }
}

void ui_show(int set_step, int current_page, int mode) {
  int current_note = set_step % 16;
  u8g2.print(current_page);

  if (current_note % 16 == 0) {
    current_page++;
  }
  for (int i = 0; i < 18; i++) {
    if (sequencer_note[current_note] != 0) {
      if (note = current_note) {
        if (mode == SQUENCER_WORK_MODE_PLAY) {
          //
        } else if (mode == SQUENCER_WORK_MODE_SET_TAPE) {
          //
        } else if (mode == SQUENCER_WORK_MODE_SET_NOTE) {
          //
        }
      }
    } else {
      // print the box
    }
  }
}

void squencer_main() {
  static unsigned long lastStepTime = 0;
  unsigned long currentTime = millis();

  if (currentTime - lastStepTime > noteDuration) {
    if (playSequence == SQUENCER_WORK_MODE_SET) {
      // sequencer setting mode
      int set_step = 0;
      int current_squencer_page = 1;
      if (key_pressed == 0) {
        ui_show(set_step, current_squencer_page, SQUENCER_WORK_MODE_SET_TAPE);
        if (encoder_read() == SQUENCER_NEXT_NOTE) {
          set_step++;
        } else if (encoder_read() == SQUENCER_PRE_NOTE) {
          set_step--;
        }
      } else {
        int result = keypadread_note();
        sequencer_note[result] = sequz_midi_map[result];
        ui_show(set_step, current_squencer_page, SQUENCER_WORK_MODE_SET_NOTE);
      }
    } else if (playSequence == SQUENCER_WORK_MODE_PLAY) {
      // sequencer run mode
      if (sequencer_note[current_note]) != 0)
      {
          playStep(sequencer_note[current_note]);
      }
      currentStep++;
    }
  }
  // currentStep = (currentStep + 1) % numKeys;
}
