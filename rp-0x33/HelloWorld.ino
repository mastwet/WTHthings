#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <semphr.h>
#include <event_groups.h>
#include <map>
#include <EEPROM.h>

TaskHandle_t TaskLed;
TaskHandle_t TaskEcKey;
TaskHandle_t TaskMain;

SemaphoreHandle_t xMutex;


// 定义用于长按检测的定时器句柄
TimerHandle_t longPressTimer;

EventGroupHandle_t eventGroup;
const EventBits_t BIT_SHORT_PRESS = (1 << 0);
const EventBits_t BIT_LONG_PRESS = (1 << 1);



#include "pianokey.h"

#include <Adafruit_NeoPixel.h>
#define PIN 9        // On Trinket or Gemma, suggest changing this to 1
#define NUMPIXELS 9  // Popular NeoPixel ring size
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


// #include <Adafruit_TinyUSB.h>
// #include <MIDI.h>

// // USB MIDI object
// Adafruit_USBD_MIDI usb_midi;

// MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI);

#include <U8g2lib.h>
#include <Wire.h>
U8G2_SH1106_128X32_VISIONOX_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);


#include "Adafruit_Keypad.h"
const byte ROWS = 3;  // rows
const byte COLS = 3;  // columns
//define the symbols on the buttons of the keypads
char keys[ROWS][COLS] = {
  { 1, 2, 3 },
  { 4, 5, 6 },
  { 7, 8, 9 }
};
byte colPins[ROWS] = { 7, 6, 8 };     //connect to the row pinouts of the keypad
byte rowPins[COLS] = { 10, 11, 12 };  //connect to the column pinouts of the keypad

int setting_key_push = 0;

Adafruit_Keypad customKeypad = Adafruit_Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

unsigned int midi_keymap[] = {
  127, 127, 127,
  127, 127, 127,
  127, 127, 127
};


#define CLK 15
#define DT 14
#define SW 13

#include <Bounce2.h>

Bounce2::Button button = Bounce2::Button();

int counter = 0;
int aState;
int aLastState;
int bState;
int bLastState;

void draw_ui();
void draw_pixel();
void read_encoder();
void colorWipe(uint32_t color, int wait);

String string_show = "hello RP";
String string_show2 = "";

#include "rp_setting.h"

int key_note_map[9] = {
  23, 24, 25,
  26, 27, 28,
  29, 30, 31
};

int key_cc_map[9] = {
  100, 100, 100,
  100, 100, 100,
  100, 100, 100
};

String function_description[9] = {
  "user1","user2","user3",
  "ctrl left","loop","ctrl right",
  "record","play","stop"
};

RP_Settings setting(key_note_map,key_cc_map);

typedef enum {
  NOTE,
  CC,
  CONCTRL,
  SETTINGKEY,
  ENDNUM  // 用于模式计数
} Mode;

Mode currentMode = NOTE;

void ModeScheduler();
void usb_send_message(int type, int number);
void show_midi_send_string(String str1, int type, int on_off);

void ledTask(void *pvParameters);
void encoderTask(void *pvParameters);
void draw_ui(void *pvParameters);
void main_loop(void *pvParameters);
void TaskButtonMonitor(void *pvParameters);
void TaskModeHandler(void *pvParameters);

void LongPressTimerCallback(TimerHandle_t xTimer);

void setup() {

  Serial.begin(115200);

  EEPROM.begin(512);
  setting.readFromEEPROM();

  delay(500);

  pixels.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)

  u8g2.begin();
  u8g2.setFont(u8g2_font_ncenB08_tr);  // choose a suitable font

  customKeypad.begin();
  pinMode(CLK, INPUT);  // 将编码器引脚设置为输入
  pinMode(DT, INPUT);   // 将编码器引脚设置为输入
  // pinMode(SW, INPUT_PULLUP);

  button.attach(SW, INPUT);  // USE EXTERNAL PULL-UP
  // DEBOUNCE INTERVAL IN MILLISECONDS
  button.interval(5);

  // INDICATE THAT THE LOW STATE CORRESPONDS TO PHYSICALLY PRESSING THE BUTTON
  button.setPressedState(LOW);

  aLastState = digitalRead(CLK);
  bLastState = digitalRead(DT);

  xMutex = xSemaphoreCreateMutex();
  if (xMutex == NULL) {
    while (1)
      ;
  }

  //longPressTimer = xTimerCreate("LongPressTimer", pdMS_TO_TICKS(3000), pdFALSE, (void *)0, LongPressTimerCallback);

  xTaskCreate(ledTask, "ledTask", 128, nullptr, 1, &TaskLed);
  xTaskCreate(encoderTask, "encoderTask", 128, nullptr, 1, nullptr);
  xTaskCreate(draw_ui, "uiTask", 128, nullptr, 1, nullptr);
  xTaskCreate(main_loop, "main_loop", 128, nullptr, 1, nullptr);
  xTaskCreate(TaskButtonMonitor, "Button Monitor", 100, NULL, 1, NULL);

  eventGroup = xEventGroupCreate();

  //vTaskStartScheduler();
}

void loop() {
  //   // put your main code here, to run repeatedly:
  // customKeypad.tick();

  // while(customKeypad.available()){
  //   String temp_string = "";
  //   keypadEvent e = customKeypad.read();

  //   //Serial.print((char)e.bit.KEY);
  //   if(e.bit.EVENT == KEY_JUST_PRESSED){ //Serial.println(" pressed");
  //   //Serial.println("pressed");
  //       temp_string = "Note ";
  //       temp_string += String(pianoKeys[key_note_map[e.bit.KEY - 1]].notePosition);
  //       temp_string += " On";
  //   }
  //   else if(e.bit.EVENT == KEY_JUST_RELEASED){ //Serial.println(" released");
  //   //Serial.println("released");
  //       temp_string = "Note ";
  //       temp_string += pianoKeys[key_note_map[e.bit.KEY - 1]].notePosition;
  //       temp_string += " Off";
  //   }
  //   if(temp_string != ""){
  //       if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
  //         string_show = temp_string;
  //         xSemaphoreGive(xMutex);
  //       }
  //   }
  // }

  //draw_ui();
  //draw_pixel();
  //int R = random(0, 255);
  //int G = random(0, 255);
  //int B = random(0, 255);
  //colorWipe(pixels.Color(R,   G,   B)     , 50); // Red
  //ModeScheduler();
}

int setting_in = 0;
int setting_encoder_dire = 0;
int mode = 0;
int menucurrentSelection = 0;

//当前正在设定的按键的note值
unsigned char set_ing_key_note = 0;
//当前正在设定的按键的cc值
int set_ing_key_cc = 0;

int setting_ccnote[9];
int setting_note[9];

void main_loop(void *pvParameters) {
  (void)pvParameters;
  EventBits_t receivedEvent;

  while (1) {
    receivedEvent = xEventGroupWaitBits(eventGroup, BIT_SHORT_PRESS | BIT_LONG_PRESS, pdTRUE, pdFALSE, 0);
    if ((receivedEvent & BIT_SHORT_PRESS) != 0) {
      // 短按事件发生
      if (currentMode != SETTINGKEY){
        currentMode = (Mode)((currentMode + 1) % (ENDNUM - 1));
        string_show = "switch 2 mode::";
        string_show += String((int)currentMode);
      }
      else{
        if(menucurrentSelection != 1){
          menucurrentSelection += 1;
        }
        else{
          menucurrentSelection = 0;
        }
      }
      //Serial.println("Short Press Detected");
    }
    if ((receivedEvent & BIT_LONG_PRESS) != 0) {
      // 长按事件发生
      if (currentMode != SETTINGKEY) {
        currentMode = SETTINGKEY;
        string_show = "enter menu";
      } else {
        currentMode = NOTE;
        string_show = "exit menu";
        string_show2 = "";
        setting.writeToEEPROM();
        delay(500);
      }
    }
    // put your main code here, to run repeatedly:
    if (currentMode != SETTINGKEY) {
      customKeypad.tick();
      while (customKeypad.available()) {
        String temp_string = "";
        keypadEvent e = customKeypad.read();
        switch(currentMode){
          case NOTE:
            temp_string = String(pianoKeys[key_note_map[e.bit.KEY - 1]].notePosition);
          break;
          case CC:
            temp_string = String(key_cc_map[e.bit.KEY - 1]);
          break;
          case CONCTRL:
            temp_string = function_description[e.bit.KEY - 1];
          break;
        }
        //Serial.print((char)e.bit.KEY);
        if (e.bit.EVENT == KEY_JUST_PRESSED) {
          show_midi_send_string(temp_string, (int)currentMode, 1);
        } else if (e.bit.EVENT == KEY_JUST_RELEASED) {
          show_midi_send_string(temp_string, (int)currentMode, 0);
        }
      }
    } else {
      String string_temp = "";
      string_temp = "Key";
      string_temp += String(setting_key_push);
      string_temp += "Note :";
      string_temp += String(pianoKeys[key_note_map[setting_key_push]].notePosition);
      string_show = string_temp;

      string_temp = "";
      string_temp = "Key";
      string_temp += String(setting_key_push);
      string_temp += "CC :";
      string_temp += String(key_cc_map[setting_key_push]);
      string_show2 = string_temp;

      customKeypad.tick();
      while (customKeypad.available()) {
        keypadEvent e = customKeypad.read();

        //Serial.print((char)e.bit.KEY);
        if (e.bit.EVENT == KEY_JUST_PRESSED) {
          setting_key_push = e.bit.KEY - 1;
        }
      }
    }
  }
}

void show_midi_send_string(String str1, int type, int on_off) {
  String temp_string;
  switch (type) {
    case 0:
      if (on_off)
        temp_string = "Note On:";
      else
        temp_string = "Note Off:";
      break;
    case 1:
      temp_string = "Control CC ";
      break;
    case 2:
      temp_string = "Function :";
      break;
  }
  temp_string += str1;
  if (temp_string != "") {
    if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
      string_show = temp_string;
      xSemaphoreGive(xMutex);
    }
  }
}

void usb_send_message(int type, int number) {
  switch (type) {
    case 0:
      break;
    case 1:
      break;
    case 2:
      break;
  }
}

void setting_key_light_prog();
void clean_led();

void ledTask(void *pvParameters) {

  int R, G, B;

  while (1) {
    switch (currentMode) {
      case NOTE:
        R = random(120, 255);
        G = random(20, 255);
        B = random(0, 255);
        colorWipe(pixels.Color(R, G, B), 50);  // Red
        break;
      case CC:
        R = random(120, 255);
        G = random(0, 255);
        B = random(100, 255);
        colorWipe(pixels.Color(R, G, B), 50);  // Red
        break;
      case CONCTRL:
        colorWipe_show_track_ctrl();
        break;
      case SETTINGKEY:
        pixels.clear();  // Set all pixel colors to 'off'
        setting_key_light_prog();
        break;
    }
  }
}

void setting_key_light_prog() {
  pixels.setPixelColor(setting_key_push, pixels.Color(0, 255, 255));
  pixels.show();
}


void encoderTask(void *pvParameters) {
  while (1) {
    read_encoder();
  }
}


void draw_ui(void *pvParameters) {

  while (1) {
    u8g2.clearBuffer();  // clear the internal memory
    if (currentMode != SETTINGKEY) {
      u8g2.drawStr(0, 12, string_show.c_str());
    } else {
      for (int i = 0; i < 2; i++) {
        if (i == menucurrentSelection) {
          // 绘制反色背景
          u8g2.setDrawColor(1);
          u8g2.drawBox(0, i * 16, 128, 16);
          u8g2.setDrawColor(0);  // 设置绘图颜色为黑色，实现反色效果
        } else {
          u8g2.setDrawColor(1);  // 设置绘图颜色为正常
        }
        switch (i) {
          case 0:
            u8g2.drawStr(0, 12, string_show.c_str());
            break;
          case 1:
            u8g2.drawStr(0, 28, string_show2.c_str());
            break;
        }
      }
    }
    u8g2.sendBuffer();  // transfer internal memory to the display
  }
}

void read_encoder() {
  String temp_string;
  aState = digitalRead(CLK);   // CLK的输出状态0/1
  bState = digitalRead(DT);    // DT的输出状态0/1
  int *set_pt;
  int current_set_mode_max = 0;

  if(currentMode != SETTINGKEY){

  if (aState != aLastState) {  //变化检测
    delay(5);                  //消抖
    if (aState != bState) {    // 如果A!=B，则表示编码器正在顺时针旋转
      counter++;
    } else {  // 如果A=B，则表示编码器正在逆时针旋转
      counter--;
    }
    if (counter > 100) {
      counter = 100;
    } else if (counter < 0) {
      counter = 0;
    }

    temp_string = "volume:";
    temp_string += String(counter);
    if (temp_string != "") {
      if (xSemaphoreTake(xMutex, portMAX_DELAY)) {
        string_show = temp_string;
        xSemaphoreGive(xMutex);
      }
    }
    //Serial.println(counter);
  }

  }
  else{
    if(menucurrentSelection == 0){
      set_pt = &key_note_map[setting_key_push];
      current_set_mode_max = 872;
    }
    else if(menucurrentSelection == 1){
      set_pt = &key_cc_map[setting_key_push];
      current_set_mode_max = 127;
    }
    if (aState != aLastState) {  //变化检测
      delay(5);                  //消抖
      if (aState != bState) {    // 如果A!=B，则表示编码器正在顺时针旋转
        *set_pt = *set_pt + 1 ;
      } else {  // 如果A=B，则表示编码器正在逆时针旋转
        *set_pt = *set_pt - 1 ;
      }
      if (*set_pt > current_set_mode_max) {
        *set_pt = current_set_mode_max ;
      } else if (*set_pt < 0) {
        *set_pt = 0;
      }
  }
  }
  aLastState = aState;
}


// void draw_pixel(){
//   pixels.clear(); // Set all pixel colors to 'off'

//   for(int i=0; i<NUMPIXELS; i++) { // For each pixel...
//     pixels.setPixelColor(i, pixels.Color(0, 150, 0));
//     pixels.show();   // Send the updated pixel colors to the hardware.
//     delay(100); // Pause before next pass through loop
//   }
// }

void colorWipe(uint32_t color, int wait) {
  for (int i = 0; i < pixels.numPixels(); i++) {  // For each pixel in strip...
    pixels.setPixelColor(i, color);               //  Set pixel's color (in RAM)
    pixels.show();                                //  Update strip to match
    delay(wait);                                  //  Pause for a moment
  }
}

// R = 100;
// G = 200;
// B = 150;
// colorWipe(pixels.Color(R, G, B), 50);  // Red

void colorWipe_show_track_ctrl(){
  int wait = 50;
  for (int i = 0; i < 3; i++) {  // For each pixel in strip...
    pixels.setPixelColor(i, pixels.Color(255, 255, 255));               //  Set pixel's color (in RAM)
    pixels.show();                                //  Update strip to match
    delay(wait);                                  //  Pause for a moment
  }
  pixels.setPixelColor(3, pixels.Color(255, 255, 0));
  pixels.show(); 
  delay(wait);

  pixels.setPixelColor(4, pixels.Color(255, 0, 255));
  pixels.show(); 
  delay(wait);

  pixels.setPixelColor(5, pixels.Color(255, 255, 0));
  pixels.show(); 
  delay(wait);

  pixels.setPixelColor(6, pixels.Color(255, 20, 30));
  pixels.show(); 
  delay(wait);

  pixels.setPixelColor(7, pixels.Color(40, 255, 30));
  pixels.show(); 
  delay(wait);

  pixels.setPixelColor(8, pixels.Color(60, 30, 255));
  pixels.show(); 
  delay(wait);
}

void LongPressTimerCallback(TimerHandle_t xTimer) {
  Serial.println("Long Press Detected - Entering Menu");
  // 处理进入菜单的逻辑
}

bool sw_sta = false;


void TaskButtonMonitor(void *pvParameters) {
  unsigned long pressStartTime = 0;
  const int LONG_PRESS_TIME = 1000;
  for (;;) {
    button.update();
    if (button.fell()) {
      Serial.println("pressed");
      pressStartTime = millis();
    }
    if (button.rose()) {
      if (millis() - pressStartTime > LONG_PRESS_TIME) {
        // 长按事件
        //Serial.println("Long Press Detected");
        xEventGroupSetBits(eventGroup, BIT_LONG_PRESS);
      } else {
        // 短按事件
        //Serial.println("Short Press Detected");
        xEventGroupSetBits(eventGroup, BIT_SHORT_PRESS);
      }
    }
  }
}
