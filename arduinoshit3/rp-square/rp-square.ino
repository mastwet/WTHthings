

#include <Adafruit_Keypad.h>

#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#include <MIDI.h>


// USB MIDI object
Adafruit_USBD_MIDI usb_midi;

// Create a new instance of the Arduino MIDI Library,
// and attach usb_midi as the transport.
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI);


// Define the row and column pins
const byte ROWS = 8; // Eight rows
const byte COLS = 8; // Eight columns

// Define the keymap as a 8x8 matrix
byte hexKeys[ROWS][COLS] = {
  {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07},
  {0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F},
  {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17},
  {0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F},
  {0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27},
  {0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F},
  {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37},
  {0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F}
};

int piano_keys[8][8] = {
    {2, 0, 1, 0, 1, 1, 0, 1},
    {0, 1, 0, 1, 2, 0, 1, 0},
    {1, 1, 0, 1, 0, 1, 0, 1},
    {2, 0, 1, 0, 1, 1, 0, 1},
    {0, 1, 0, 1, 2, 0, 1, 0},
    {1, 1, 0, 1, 0, 1, 0, 1},
    {2, 0, 1, 0, 1, 1, 0, 1},
    {0, 1, 0, 1, 2, 0, 1, 0}
};

// Define the row and column pin arrays
// Update these pin numbers based on your actual wiring
byte rowPins[ROWS] = {2, 3, 4, 5, 7, 8, 9, 10};
byte colPins[COLS] = {21, 17, 1, 6, 12, 13, 14,15};

#include <Adafruit_NeoPixel.h>

#define PIN        11 // NeoPixel矩阵连接的引脚
#define NUMPIXELS 64 // 矩阵中的LED数量

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// Initialize the keypad
Adafruit_Keypad customKeypad = Adafruit_Keypad(makeKeymap(hexKeys), rowPins, colPins, ROWS, COLS);

void lightUpMatrix();
uint32_t getColor(int keyType);

#include <ShiftIn.h> // 引入ShiftIn库

// 创建一个ShiftIn实例，参数2表示有2个74HC165芯片级联
ShiftIn<2> shift;

// 定义芯片相关的引脚
const int pLoadPin = 47; // 并行加载或复位引脚
const int clockEnablePin = 42; // 时钟使能引脚（如果库支持的话）
const int dataPin = 45; // 数据输出引脚（连接至第一个74HC165的SER引脚）
const int clockPin = 48; // 时钟引脚（连接至所有74HC165的CLK引脚）

void echoValues();

int last_sta[16] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}; 

void setup() {
  #if defined(ARDUINO_ARCH_MBED) && defined(ARDUINO_ARCH_RP2040)
  // Manual begin() is required on core without built-in support for TinyUSB such as mbed rp2040
    TinyUSB_Device_Init(0);
  #endif

  MIDI.begin(MIDI_CHANNEL_OMNI);

  while( !TinyUSBDevice.mounted() ) delay(1);

  pixels.begin(); // 初始化NeoPixel矩阵
  lightUpMatrix();

  customKeypad.begin();

  // 初始化ShiftIn对象
  shift.begin(pLoadPin, clockEnablePin, dataPin, clockPin);
  //Serial.begin(9600);
}

const int midinote_base = 24;

void loop() {
  // 更新所有按键状态并检查是否有按键状态变化
  if(shift.update()) {
    echoValues();
  }
  customKeypad.tick();

  while (customKeypad.available()) {
    keypadEvent e = customKeypad.read();
    if (e.bit.EVENT == KEY_JUST_PRESSED) {
      MIDI.sendNoteOn(midinote_base+e.bit.KEY, 127, 1);
      //Serial.print("Key Pressed: 0x");
      //Serial.println(e.bit.KEY, HEX);
    } else if (e.bit.EVENT == KEY_JUST_RELEASED) {
      MIDI.sendNoteOff(midinote_base+e.bit.KEY, 0, 1);
      //Serial.print("Key Released: 0x");
      //Serial.println(e.bit.KEY, HEX);
    }
  }
}

void lightUpMatrix() {
  for(int i = 0; i < 8; i++) {
    for(int j = 0; j < 8; j++) {
      int keyType = piano_keys[i][j];
      int pixelNum = i * 8 + j;
      uint32_t color = getColor(keyType);
      pixels.setPixelColor(pixelNum, color);
    }
  }
  pixels.show(); // 更新NeoPixel矩阵的状态
}

uint32_t getColor(int keyType) {
  switch(keyType) {
    case 0: return pixels.Color(255, 255, 255); // 白灯（半音）
    case 1: return pixels.Color(0, 0, 255);     // 蓝灯（全音）
    case 2: return pixels.Color(255, 255, 0);   // 黄灯（全音C）
    default: return pixels.Color(0, 0, 0);      // 关灯
  }
}

int remap(int num){
  // 输入序列
  //int input[] = {12, 13, 14, 15, 11, 10, 9, 8, 4, 5, 6, 7, 3, 2, 1, 0};
  int input[] = {15,14,13,12,8,9,10,11,7,6,5,4,0,1,2,3};
  return input[num];
}

// 假设这是16个按键对应的16个不同的CC编号
const int midiCcNumbers[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

void echoValues() {
  for(int i = 0; i < shift.getDataWidth(); i++) {
    if(last_sta[i] != shift.state(i)){
      if(shift.state(i) == LOW){
        MIDI.sendControlChange(midiCcNumbers[i], 127, MIDI_CHANNEL_OMNI); // 发送CC信号
      }
      else{
        MIDI.sendControlChange(midiCcNumbers[i], 0, MIDI_CHANNEL_OMNI); // 发送CC信号
      }
      //Serial.printf("%d pressed!",remap(i));
    }
    last_sta[i] = shift.state(i);
  }
  Serial.println();

}