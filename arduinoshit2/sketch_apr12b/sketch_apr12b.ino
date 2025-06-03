#include <MIDI.h>

#define DRUM_MASHINE_NEXT_NOTE 1
#define DRUM_MASHINE_PRE_NOTE 0
#define DRUM_MASHINE_WORK_MODE_SET 0
#define DRUM_MASHINE_WORK_MODE_PLAY 1

MIDI_CREATE_DEFAULT_INSTANCE();

// Sequencer settings
int sequence[numKeys] = {0};
int currentStep = 0;
bool playSequence = false;
unsigned long noteDuration;

#define NUM_DRUM_SOUNDS 4 // Number of drum sounds (e.g., kick, snare, clap, hi-hat)
#define NUM_STEPS 16      // Number of steps in the sequence

typedef enum
{
  KICK = 0,
  SNARE,
  CLAP,
  HIHAT
} DrumSound;

typedef enum
{
  OPEN = 0,
  CLOSE
} DrumSound;

typedef struct
{
  DrumSound steps[NUM_STEPS]; // Array of sequence steps
  int channel;                // MIDI channel
} DrumSequence;

typedef struct
{
  DrumSequence kick;  // Array of sequence steps
  DrumSequence snare; // Array of sequence steps
  DrumSequence clap;  // Array of sequence steps
  DrumSequence hihat; // Array of sequence steps
} DrumMashineStruct;

// Example of initializing a drum sequence
DrumMashineStruct drum_mashine = {
    .kick = {0},
    .snare = {0},
    .clap = {0},
    .hihat = {0}};

typedef struct
{
  int num;
  int rotation;
} EncoderMessage;

// Initialize MIDI
MIDI.begin(MIDI_CHANNEL_OMNI);

void setup()
{
  // Initialize serial
  Serial.begin(9600);
  MIDI.begin(MIDI_CHANNEL_OMNI);
}

void loop()
{
}

// read encoder.if encoder left turn, return 1.if encoder right turn, return -1.if encoder not turn, return 0.
int encoder_key()
{
  return key;
}

void playStep()
{
  int note[16];

  note[0] = drum_mashine.kick.steps[currentStep];
  note[1] = drum_mashine.snare.steps[currentStep];
  note[2] = drum_mashine.clap.steps[currentStep];
  note[3] = drum_mashine.hihat.steps[currentStep];

  for (int i = 0; i < 16; i++)
  {
    if (note[i] != 0)
    {
      MIDI.sendNoteOn(note[i], 127, 1);
      MIDI.sendNoteOff(note[i], 0, 1);
    }
  }
}

void updateBPM()
{
  // show bpm on u8g2 display oled12864,encoder input
  int encoder_value = encoder_read();
  if (encoder_value != 0)
  {
    if (encoder_value == 1)
    {
      bpm++;
    }
    else if (encoder_value == -1)
    {
      bpm--;
    }
    noteDuration = 60000.0 / (bpm * (numKeys / 4.0)); // Calculate the note duration based on BPM and number of steps

    u8g2.clearBuffer();
    u8g2.setCursor(0, 10);
    u8g2.print("BPM:");
    u8g2.setCursor(0, 20);
    u8g2.print(bpm);
    u8g2.sendBuffer();
  }
}

void ui_show(int set_step, int current_page, int mode)
{
  int current_note = set_step % 16;
  u8g2.print(current_page);

  if (current_note % 16 == 0)
  {
    current_page++;
  }
  for (int i = 0; i < 18; i++)
  {
    if (sequencer_note[current_note] != 0)
    {
      if (note = current_note)
      {
        if (mode == DRUM_MASHINE_WORK_MODE_PLAY)
        {
          //
        }
        else if (mode == DRUM_MASHINE_WORK_MODE_SET_TAPE)
        {
          //
        }
        else if (mode == DRUM_MASHINE_WORK_MODE_SET_CHANNEL)
        {
          //
        }
      }
    }
    else
    {
      // print the box
    }
  }
}

void drum_mashine_main()
{
  static unsigned long lastStepTime = 0;
  unsigned long currentTime = millis();

  if (currentTime - lastStepTime > noteDuration)
  {
    if (playSequence == SQUENCER_WORK_MODE_SET)
    {
      // sequencer setting mode
      int set_step = 0;
      int set_channel = 0;
      int current_squencer_page = 1;
      if (key_pressed == 0)
      {
        ui_show(set_step, current_squencer_page, SQUENCER_WORK_MODE_SET_TAPE);
        if (encoder_read().num = 0)
        {
          if (encoder_read() == SQUENCER_NEXT_NOTE)
          {
            set_step++;
          }
          else if (encoder_read() == SQUENCER_PRE_NOTE)
          {
            set_step--;
          }
        }
        if (encoder_read().num = 1)
        {
          if (encoder_read() == SQUENCER_NEXT_NOTE)
          {
            set_channel++;
          }
          else if (encoder_read() == SQUENCER_PRE_NOTE)
          {
            set_channel--;
          }
        }
      }
      else
      {
        switch (set_channel)
        {
        case 0:
          ~drum_mashine.clap[set_step];
          break;
        case 1:
          ~drum_mashine.hihat[set_step];
          break;
        case 2:
          ~drum_mashine.kick[set_step];
          break;
        case 3:
          ~drum_mashine.snare[set_step];
          break;
        }
        ui_show(set_step, current_squencer_page, SQUENCER_WORK_MODE_SET_NOTE);
      }
    }
    else if (playSequence == SQUENCER_WORK_MODE_PLAY)
    {
      // sequencer run mode
      playStep(current_note);
      currentStep++;
    }
  }
  // currentStep = (currentStep + 1) % numKeys;
}
