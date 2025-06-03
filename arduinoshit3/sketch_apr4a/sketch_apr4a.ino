#include <MIDI.h>
#include <FreeRTOS.h>
#include <semphr.h>

const int bpmPin = 2; // BPM input pin
const int rootNotePin = 3; // Root note input pin
const int arpInterval = 125; // Arpeggio interval time (ms)

int bpm = 120; // Initial BPM value
int rootNote = 60; // Initial root note value
int arpNote[4] = {0, 4, 7, 12}; // Arpeggio notes: root, third, fifth and octave
int arpPos = 0; // Arpeggio position counter
int noteDuration = 500; // Note duration (ms)
int velocity = 127; // Note velocity

MIDI_CREATE_DEFAULT_INSTANCE(); // Create MIDI instance

SemaphoreHandle_t sema; // Semaphore for synchronization

void onTimer() {
  // Wait for semaphore to be available
  xSemaphoreTake(sema, portMAX_DELAY);
  
  // Play the next note in the arpeggio sequence
  MIDI.sendNoteOn(rootNote + arpNote[arpPos], velocity, 1);
  arpPos++;
  if (arpPos >= 4) {
    arpPos = 0;
  }
  
  // Release semaphore to allow main task to continue
  xSemaphoreGive(sema);
}

void arpeggiateTask(void *parameter) {
  while (true) {
    // Wait for semaphore to be available
    xSemaphoreTake(sema, portMAX_DELAY);
    
    // Calculate note interval time based on BPM
    int interval = 60000 / bpm / 4;
    
    // Delay for the arpeggio interval time
    vTaskDelay(pdMS_TO_TICKS(interval * arpInterval));
    
    // Release semaphore to allow timer interrupt to continue
    xSemaphoreGive(sema);
  }
}

void setup() {
  pinMode(bpmPin, INPUT);
  pinMode(rootNotePin, INPUT);
  MIDI.begin(MIDI_CHANNEL_OMNI);

  // Create semaphore
  sema = xSemaphoreCreateBinary();
  xSemaphoreGive(sema); // Initialize semaphore to available
  
  // Configure hardware timer
  ESP32TimerInterrupt timer(0);
  int interval = 60000 / bpm / 4; // Calculate note interval time based on BPM
  timer.attachInterruptInterval(interval * 1000, onTimer);
  
  // Create arpeggiate task
  xTaskCreate(arpeggiateTask, "ArpeggiateTask", 2048, NULL, 1, NULL);
}

void loop() {
  // Read BPM and root note input values
  bpm = map(analogRead(bpmPin), 0, 1023, 40, 240); // Map input to BPM value between 40 and 240
  rootNote = map(analogRead(rootNotePin), 0, 1023, 0, 127); // Map input to MIDI note value between 0 and 127
  
  // Release semaphore to allow arpeggiate task to run
  xSemaphoreGive(sema);
}