#include <Arduino.h>
#include <RoxMux.h>
// #include <Adafruit_TinyUSB.h>  // 暂时注释USB库
// #include <MIDI.h>             // 暂时注释MIDI库

// MIDI相关设置（已注释）
// Adafruit_USBD_MIDI usb_midi;
// MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI);

#include <Versatile_RotaryEncoder.h>

#define clk1 21  // (A3)
#define dt1 20   // (A2)
#define sw1 6    // (A4)

#define clk2 0  // (A3)
#define dt2 1   // (A2)
#define sw2 27  // (A4)

#define clk3 2  // (A3)
#define dt3 3   // (A2)
#define sw3 26  // (A4)



// 74HC165设置
#define MUX_TOTAL 3
Rox74HC165<MUX_TOTAL> mux;

#define NUM_BUTTONS 24
RoxButton buttons[NUM_BUTTONS];

// 引脚定义
#define PIN_DATA  12
#define PIN_LOAD  10
#define PIN_CLK   11

// 音符映射表
const byte noteMap[NUM_BUTTONS] = {
  60, 61, 62, 63, 64, 65, 66, 67,  // C4到G4
  68, 69, 70, 71, 72, 73, 74, 75,  // G#4到D5
  76, 77, 78, 79, 80, 81, 82, 83   // D#5到B5
};

// 状态缓存
bool previousState[NUM_BUTTONS] = {0};

// 新增全局变量：当前扫描的编码器编号（1-3）
int currentEncoder = 0;  // 0表示未扫描任何编码器

// SET READING PINS ACCORDINGLY TO YOUR ENCODER TO BOARD CONNECTIONS
// ... [原有的宏定义保持不变]

const int buttonPins[] = {9, 8, 7}; 
const int numButtons = 3;      
int lastButtonStates[numButtons];   

// Create a global pointer for the encoder object
Versatile_RotaryEncoder *versatile_encoder1;
Versatile_RotaryEncoder *versatile_encoder2;
Versatile_RotaryEncoder *versatile_encoder3;

void scan_encoder();
void scan_button();
void scan_pianoboard();

void handleRotate(int8_t rotation);
void handlePress();

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10); // 等待串口连接
  
  // MIDI初始化（已注释）
  // #if defined(ARDUINO_ARCH_MBED) && defined(ARDUINO_ARCH_RP2040)
  // TinyUSB_Device_Init(0);
  // #endif
  // usb_midi.setStringDescriptor("DIY MIDI Keyboard");
  // MIDI.begin(MIDI_CHANNEL_OMNI);
  // while(!TinyUSBDevice.mounted()) delay(1);

  versatile_encoder1 = new Versatile_RotaryEncoder(clk1, dt1, sw1);
  versatile_encoder2 = new Versatile_RotaryEncoder(clk2, dt2, sw2);
  versatile_encoder3 = new Versatile_RotaryEncoder(clk3, dt3, sw3);

  // Load to the encoder all nedded handle functions here (up to 9 functions)
  versatile_encoder1->setHandleRotate(handleRotate);
  versatile_encoder1->setHandlePress(handlePress);

  versatile_encoder2->setHandleRotate(handleRotate);
  versatile_encoder2->setHandlePress(handlePress);

  versatile_encoder3->setHandleRotate(handleRotate);
  versatile_encoder3->setHandlePress(handlePress);


  // 初始化74HC165
  mux.begin(PIN_DATA, PIN_LOAD, PIN_CLK);
  
  // 初始化按钮
  for(int i=0; i<NUM_BUTTONS; i++) {
    buttons[i].begin();
  }

    // 初始化所有按钮引脚为输入模式（启用内部上拉电阻）
  for (int i = 0; i < numButtons; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    lastButtonStates[i] = HIGH; // 初始状态设为高电平
  }

  Serial.println("System Ready");
}

void loop() {
  // MIDI.read(); // 已注释MIDI通信
  scan_pianoboard();
  scan_encoder();
  scan_button();
}

void scan_pianoboard(){
  mux.update();
  
  for(int i=0; i<NUM_BUTTONS; i++) {
    // 更新按钮状态（消抖时间50ms）
    buttons[i].update(mux.read(i), 50, LOW);

    // 检测按下事件
    if(buttons[i].pressed()) {
      // MIDI.sendNoteOn(noteMap[i], 127, 1);
      Serial.print("[PRESS] Key: ");
      Serial.print(i);
      Serial.print(" -> Note: ");
      Serial.println(noteMap[i]);
      previousState[i] = true;
    }
    // 检测释放事件
    else if(buttons[i].released()) {
      // MIDI.sendNoteOff(noteMap[i], 0, 1);
      Serial.print("[RELEASE] Key: ");
      Serial.print(i);
      Serial.print(" -> Note: ");
      Serial.println(noteMap[i]);
      previousState[i] = false;
    }
  }
}

void scan_button(){
    for (int i = 0; i < numButtons; i++) {
    int currentState = digitalRead(buttonPins[i]);
    
    // 检测状态变化
    if (currentState != lastButtonStates[i]) {
      delay(50); // 简单的防抖动延时
      
      // 再次确认状态
      currentState = digitalRead(buttonPins[i]);
      if (currentState != lastButtonStates[i]) {
        // 更新状态并输出
        lastButtonStates[i] = currentState;
        
        if (currentState == LOW) {
          Serial.print("按钮 ");
          Serial.print(buttonPins[i]);
          Serial.println(" 被按下");
        } else {
          Serial.print("按钮 ");
          Serial.print(buttonPins[i]);
          Serial.println(" 被释放");
        }
      }
    }
  }
}

void scan_encoder(){
    // 扫描编码器1时更新全局变量
  currentEncoder = 1;
  if (versatile_encoder1->ReadEncoder()) {
    // 可以在这里添加编码器1专用的处理
  }

  // 扫描编码器2时更新全局变量
  currentEncoder = 2;
  if (versatile_encoder2->ReadEncoder()) {
    // 可以在这里添加编码器2专用的处理
  }

  // 扫描编码器3时更新全局变量
  currentEncoder = 3;
  if (versatile_encoder3->ReadEncoder()) {
    // 可以在这里添加编码器3专用的处理
  }
  currentEncoder = 0; // 可选：扫描完成后重置状态
}

// 修改后的回调函数：使用全局变量判断编码器
void handleRotate(int8_t rotation) {
  Serial.print("#");
  Serial.print(currentEncoder);  // 动态显示当前编码器
  Serial.print(" Rotated: ");
  rotation > 0 ? Serial.println("Right") : Serial.println("Left");
}

void handlePress() {
  Serial.print("#");
  Serial.print(currentEncoder);  // 动态显示当前编码器
  Serial.println(".1 Pressed");
}
