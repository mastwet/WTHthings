#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE();

#define BTN_STOP_ALARM 0

hw_timer_t *timer = NULL;
volatile SemaphoreHandle_t timerSemaphore;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

volatile uint32_t isrCounter = 0;
volatile uint32_t lastIsrAt = 0;

// Define MIDI note values
#define NOTE_C4 60
#define NOTE_E4 64
#define NOTE_G4 67
#define NOTE_C5 72

// Define your MIDI sequence here
const int midiNotes[] = {
  NOTE_C4,
  NOTE_E4,
  NOTE_G4,
  NOTE_C5,
  NOTE_G4,
  NOTE_E4,
  NOTE_C4,
  NOTE_C4,
  -1  // Rest
};

const int midiDurations[] = {
  500,  // Duration for C4
  500,  // Duration for E4
  500,  // Duration for G4
  500,  // Duration for C5
  500,  // Duration for G4
  500,  // Duration for E4
  500,  // Duration for C4
  500   // Duration for rest
};

int sequenceIndex = 0;
bool isPlaying = true;

// BPM and related variables
int bpm = 128;
int timerInterval = 60000000 / bpm;

void ARDUINO_ISR_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  isrCounter++;
  lastIsrAt = millis();
  portEXIT_CRITICAL_ISR(&timerMux);
  xSemaphoreGiveFromISR(timerSemaphore, NULL);
}

void setup() {
  Serial.begin(115200);
  //MIDI.begin(MIDI_CHANNEL_OMNI);
  //MIDI.turnThruOff();
  pinMode(BTN_STOP_ALARM, INPUT);

  timerSemaphore = xSemaphoreCreateBinary();

  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, (timerInterval/4), true);
  timerAlarmEnable(timer);
}

void loop() {
  if (xSemaphoreTake(timerSemaphore, 0) == pdTRUE) {
    //Handle MIDI events here
    if(midiNotes[sequenceIndex+1] == -1){
      sequenceIndex = 0;
    }
      if (sequenceIndex < sizeof(midiNotes) / sizeof(midiNotes[0])) {
        int note = midiNotes[sequenceIndex];
        int duration = midiDurations[sequenceIndex];
        if (note >= 0) {
          Serial.printf("%d",note);
          if(sequenceIndex == 0){
            Serial.print("-------->a new 4/4 beat.");
          }
          //MIDI.sendNoteOn(note, 127, 1); // Channel 1 with maximum velocity
          
          Serial.println();
        } else {
          // Rest
        }
        int sensorValue = analogRead(1);
        float bpmtp = map(sensorValue, 0, 4096, 1, 139);
        //if(bpmtp<=0) bpmtp=1;
        if(abs((int)bpmtp-bpm)>1){
          Serial.printf("change bpm:%f",bpmtp);
          bpm = (int)bpmtp;
          timerAlarmWrite(timer, ( 60000000 / bpm /4), true);
        }
        //delay(duration);
        // if (note >= 0) {
        //   //MIDI.sendNoteOff(note, 0, 1);
        //   Serial.printf("%d",note);
        //   Serial.println();
        // }
        sequenceIndex++;
      }
    
    //Serial.println("123");
  }

  // if (digitalRead(BTN_STOP_ALARM) == LOW) {
  //   if (timer) {
  //     timerEnd(timer);
  //     timer = NULL;
  //   }
  // }

  // // Handle start/stop and reset buttons
  // if (digitalRead(BTN_STOP_ALARM) == LOW) {
  //   if (isPlaying) {
  //     isPlaying = false; // Stop playback
  //   } else {
  //     isPlaying = true; // Start playback
  //   }
  //   delay(200); // Debounce
  // }

  // Add code to reset the sequence if needed
  // You can use another button or condition to reset the sequence.
}