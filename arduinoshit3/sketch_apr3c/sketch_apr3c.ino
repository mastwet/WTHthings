#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#include <MIDI.h>


const int bpmPin = 2;          // BPM输入引脚
const int rootNotePin = 3;     // 根音输入引脚
const int noteDuration = 500;  // 音符持续时间（毫秒）
const int velocity = 127;      // 音符速度

int bpm = 120;      // 初始BPM值
int rootNote = 60;  // 初始根音值
int noteInterval;   // 音符间隔时间（毫秒）

// USB MIDI object
Adafruit_USBD_MIDI usb_midi;

// Create a new instance of the Arduino MIDI Library,
// and attach usb_midi as the transport.
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI);

void setup() {
#if defined(ARDUINO_ARCH_MBED) && defined(ARDUINO_ARCH_RP2040)
  // Manual begin() is required on core without built-in support for TinyUSB such as mbed rp2040
  TinyUSB_Device_Init(0);
#endif
  MIDI.begin(MIDI_CHANNEL_OMNI);
  while( !TinyUSBDevice.mounted() ) delay(1);
}

void loop() {
  noteInterval = 60000 / bpm / 4;  // 计算音符间隔时间

  // 播放琶音音序
  MIDI.sendNoteOn(rootNote, velocity, 1);
  delay(noteInterval);
  MIDI.sendNoteOff(rootNote, velocity, 1);
  MIDI.sendNoteOn(rootNote + 4, velocity, 1);
  delay(noteInterval);
  MIDI.sendNoteOff(rootNote + 4, velocity, 1);
  MIDI.sendNoteOn(rootNote + 7, velocity, 1);
  delay(noteInterval);
  MIDI.sendNoteOff(rootNote + 7, velocity, 1);
  MIDI.sendNoteOn(rootNote + 4, velocity, 1);
  delay(noteInterval);
  MIDI.sendNoteOff(rootNote + 4, velocity, 1);
}
