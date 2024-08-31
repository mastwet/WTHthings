
#define DEGUG 1

// pins for 74HC165
#define PIN_DATA 37  // pin 9 on 74HC165 (DATA)
#define PIN_LOAD 35  // pin 1 on 74HC165 (LOAD)
#define PIN_CLK 36   // pin 2 on 74HC165 (CLK))

#define PIN2_DATA 16  // pin 9 on 74HC165 (DATA)
#define PIN2_LOAD 17  // pin 1 on 74HC165 (LOAD)
#define PIN2_CLK 18   // pin 2 on 74HC165 (CLK))

bool pinState[8];
bool key_pinState[8];

bool data2[8];

#include <U8g2lib.h>
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE, /* clock=*/4, /* data=*/2);

#include <FastLED.h>
#define LED_PIN 11
#define NUM_LEDS 16
CRGB leds[NUM_LEDS];

uint8_t hue = 0;
uint8_t sat = 255;
uint8_t val = 0;
boolean fadeDirection = 1;  // [1=fade up, 0=fade down]
/*
#ifndef DEBUG

#include <Adafruit_TinyUSB.h>
#include <MIDI.h>
Adafruit_USBD_MIDI usb_midi;

#include <BLEMidi.h>
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI);

#endif
*/

#include <WiFi.h>
#include "update_info.h"

// Define Queue handle
QueueHandle_t QueueHandle_usb;
QueueHandle_t QueueHandle_oled;

const int QueueElementSize = 10;

EventGroupHandle_t eventGroup;
#define TASK_SWITCH_TO_MENU_BIT (1 << 0)
//#define TASK_B_BIT (1 << 1)

// Declare a semaphore handle.
SemaphoreHandle_t xSemaphore = NULL;

TaskHandle_t MainProgressTask_OLED;  
TaskHandle_t MenuProgressTask_OLED;  

TaskHandle_t LightTask;  
TaskHandle_t TimtraceTask;  
TaskHandle_t taskMidi;
TaskHandle_t taskSetting;

typedef struct {
  int type;
  int name;
  int value;
} message_t;

//type 0:Note
//type 1:ControlChange

typedef struct {
  char type;
  char note_pitch;
  uint8_t note_state;
} midi_message_t;



void ReadFromTimtrace(void *pvParameters);
void TaskWriteToMidi(void *pvParameters);
void TaskFastLED(void *pvParameters);
void TaskOLED(void *pvParameters);
void setting_program_void(void *pvParameters);

#define PACKET_START (0xFA)
#define PACKET_END (0xFF)

/*---------------------开始船新预制库体系-----------------------------*/

/*
//更新之后的船新协议体系：
//结构体MIDIMESSAGE:NOTE 音符类信号 CC 控制类信号 SYS 系统级信号 POLY 弯音信号（暂时不做）
//按键对应的的值。初版总共有5组。每一组可以通过sys级midi协议进行设置。（也可以使用原有的内存卡配置方式进行设置）

enum MIDITYPE{
  NOTE=0,
  CC=1,
  SYS=2
};

typedef unsigned int MIDIMESSAGE_T;
typedef unsigned int CHANNEL_T;

//在CC和NOTE中，都有三个值。对于打击垫场景在全局力度里面进行设置。
//CC：1、控制器编号 2、控制器值
//NOTE：1、音符 2、力度
//SYS：不带两个参数，则使用message1
struct midi_message_class{
  MIDITYPE type;
  CHANNEL_T channel;
  MIDIMESSAGE_T message1;
};

//正式写knob_bank:旋钮的预制库和pad_bank：打击垫预制库
//测试版写死

midi_message_class knob_bank[4][5]{
  {
    {CC,0,100},
    {CC,0,100},
    {CC,0,100},
    {CC,0,100},
  },
  {
    {CC,0,100},
    {CC,0,100},
    {CC,0,100},
    {CC,0,100},
  },
  {
    {CC,0,100},
    {CC,0,100},
    {CC,0,100},
    {CC,0,100},
  },
  {
    {CC,0,100},
    {CC,0,100},
    {CC,0,100},
    {CC,0,100},
  },
  {
    {CC,0,100},
    {CC,0,100},
    {CC,0,100},
    {CC,0,100},
  }
}

midi_message_class pad_bank[16][5]{
  {
    {NOTE,0,100},
    {NOTE,0,100},
    {NOTE,0,100},
    {NOTE,0,100},

    {NOTE,0,100},
    {NOTE,0,100},
    {NOTE,0,100},
    {NOTE,0,100},

    {NOTE,0,100},
    {NOTE,0,100},
    {NOTE,0,100},
    {NOTE,0,100},

    {NOTE,0,100},
    {NOTE,0,100},
    {NOTE,0,100},
    {NOTE,0,100},
  },
  {
    {NOTE,0,100},
    {NOTE,0,100},
    {NOTE,0,100},
    {NOTE,0,100},

    {NOTE,0,100},
    {NOTE,0,100},
    {NOTE,0,100},
    {NOTE,0,100},

    {NOTE,0,100},
    {NOTE,0,100},
    {NOTE,0,100},
    {NOTE,0,100},

    {NOTE,0,100},
    {NOTE,0,100},
    {NOTE,0,100},
    {NOTE,0,100},
  },
  {
    {NOTE,0,100},
    {NOTE,0,100},
    {NOTE,0,100},
    {NOTE,0,100},

    {NOTE,0,100},
    {NOTE,0,100},
    {NOTE,0,100},
    {NOTE,0,100},

    {NOTE,0,100},
    {NOTE,0,100},
    {NOTE,0,100},
    {NOTE,0,100},

    {NOTE,0,100},
    {NOTE,0,100},
    {NOTE,0,100},
    {NOTE,0,100},
  },
  {
    {NOTE,0,100},
    {NOTE,0,100},
    {NOTE,0,100},
    {NOTE,0,100},

    {NOTE,0,100},
    {NOTE,0,100},
    {NOTE,0,100},
    {NOTE,0,100},

    {NOTE,0,100},
    {NOTE,0,100},
    {NOTE,0,100},
    {NOTE,0,100},

    {NOTE,0,100},
    {NOTE,0,100},
    {NOTE,0,100},
    {NOTE,0,100},
  },
  {
    {NOTE,0,100},
    {NOTE,0,100},
    {NOTE,0,100},
    {NOTE,0,100},

    {NOTE,0,100},
    {NOTE,0,100},
    {NOTE,0,100},
    {NOTE,0,100},

    {NOTE,0,100},
    {NOTE,0,100},
    {NOTE,0,100},
    {NOTE,0,100},

    {NOTE,0,100},
    {NOTE,0,100},
    {NOTE,0,100},
    {NOTE,0,100},
  }
}
*/
/*---------------------结束船新预制库体系-----------------------------*/

int key_note_map[16] = {
  101, 102, 103, 104,
  105, 106, 107, 108,
  109, 110, 111, 112,
  113, 114, 115, 116
};

//旋钮对应的cc值。初版总共有5组。每一组可以通过sys级midi协议进行设置。（也可以使用原有的内存卡配置方式进行设置）


int current_key_page = 0;
int current_knob_page = 0;
int current_light_mode = 0;
char *light_mode_config_string[3] = {
  "Normal",
  "Fluid",
  "Shine"
};

void setup() {
  // #ifndef DEBUG
  //   BLEMidiServer.begin("RapidPulze Chocoly");

  // #if defined(ARDUINO_ARCH_MBED) && defined(ARDUINO_ARCH_RP2040)
  //   // Manual begin() is required on core without built-in support for TinyUSB such as mbed rp2040
  //   TinyUSB_Device_Init(0);
  // #endif
  //   MIDI.begin(MIDI_CHANNEL_OMNI);


  //   while (!TinyUSBDevice.mounted()) {
  //     Serial.println("Not OK!");
  //     delay(100);
  //   }

  //#else
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, 6, 5);
  // #endif

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);

  u8g2.setBusClock(800000);
  u8g2.begin();
  u8g2.setFont(u8g2_font_ncenB08_tr);

  QueueHandle_usb = xQueueCreate(QueueElementSize, sizeof(midi_message_t));
  QueueHandle_oled = xQueueCreate(QueueElementSize, sizeof(message_t));
  // Check if the queue was successfully created
  if (QueueHandle_usb == NULL || QueueHandle_oled == NULL) {
    while (1) {
      Serial.println("Queue could not be created. Halt.");
      delay(1000);  // Halt at this point as is not possible to continue
    }
  } else {
    Serial.println("Queue create sucessful.");
  }

  xTaskCreate(
    TaskFastLED, "Task RGB Light"  // A name just for humans
    ,
    2048  // The stack size can be checked by calling `uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);`
    ,
    (void *)NULL  // No parameter is used
    ,
    2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,
    &LightTask  // Task handle is not used here
  );


  xTaskCreate(
    ReadFromTimtrace, "Read Timtrace"  // A name just for humans
    ,
    2048  // The stack size can be checked by calling `uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);`
    ,
    (void *)NULL,
    2  // Priority
    ,
    &TimtraceTask  // Task handle is not used here - simply pass NULL
  );



  xTaskCreate(
    TaskWriteToMidi, "Task Write To Midi"  // A name just for humans
    ,
    2048  // The stack size can be checked by calling `uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);`
    ,
    (void *)NULL  // No parameter is used
    ,
    2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,
    &taskMidi  // Task handle is not used here
  );



  xTaskCreate(
    TaskOLED, "OLED Task"  // A name just for humans
    ,
    2048  // The stack size can be checked by calling `uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);`
    ,
    (void *)NULL,
    2  // Priority
    ,
    &MainProgressTask_OLED  // Task handle is not used here - simply pass NULL
  );

  xTaskCreate(
    task_menu, "Menu Task"  // A name just for humans
    ,
    2048  // The stack size can be checked by calling `uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);`
    ,
    (void *)NULL,
    2  // Priority
    ,
    &MenuProgressTask_OLED  // Task handle is not used here - simply pass NULL
  );


  vTaskSuspend(MenuProgressTask_OLED);

  // 创建信号量，并将计数器初始化为1 
  xSemaphore = xSemaphoreCreateCounting(1, 1);

  //eventGroup = xEventGroupCreate();
}


enum state {
  RELEASE = 0,
  PRESSED = 1,
  LONGPRESSED = 2
};

state key_state[8];

unsigned long button_pressed_time[8] = { 0 };

int packet_en = 1;

int knob_page_config = 0;
int pad_page_config = 0;
int light_mode_config = 0;
int global_light_mode = 0;

int trackctrl_direct = 0;

bool menu_in = false;

void ReadFromTimtrace(void *pvParameters) {  // This is a task.
  // put your main code here, to run repeatedly:
  byte packet[30] = { 0 };
  byte encoder_state[4] = { 0 };
  midi_message_t midimessage;
  message_t message;
  int packet_idx = 0;
  int packet_en = 1;

  int current_mode = 0;
  int current_rotation = 0;

  unsigned long menu_button_pressed_time = 0;

  //第一个按钮的按下时间。第一个按键长按进入4page box 模式
  unsigned long play_pressed_time = 0;
  //第二个按钮的按下时间。第二个按钮长按进入音序器模式
  unsigned long stop_pressed_time = 0;

  //0:unpressed
  //1:shortpressed
  //2:longpressed
  int menu_button_pressed_stata = 0;

  int ban_trans = 0;

  while (1) {


    if (menu_button_pressed_time != 0) {
      //Serial.println(millis()-menu_button_pressed_time);
      if (millis() - menu_button_pressed_time > 500) {
        //Serial.println("longpress");
        //delay(1000);
        if(!menu_in){
            vTaskSuspend(MainProgressTask_OLED);
            vTaskResume(MenuProgressTask_OLED);
            menu_in = true;
            delay(1000);
        }
        else{
            vTaskSuspend(MenuProgressTask_OLED);
            vTaskResume(MainProgressTask_OLED);
            delay(4000);
            menu_in = false;
        }
        
        
      }
      //menu_button_pressed_time = millis();
    }
    else if(play_pressed_time != 0){
      if (millis() - menu_button_pressed_time > 2000) {
          //vTaskSuspend(MainProgressTask_OLED);
          //恢复4页盒子主任务
      }
    }
    else if(stop_pressed_time != 0){
      if (millis() - menu_button_pressed_time > 2000) {
          //vTaskSuspend(MainProgressTask_OLED);
          //恢复音序器主任务
      }
    }
    //主程序
    while (Serial1.available()) {

      //串口阻塞读取程序外判断，是否菜单按下时时间达到标准。如果达到标准执行任务调度。
      byte data = Serial1.read();
      packet[packet_idx++] = data;

      if (data == 0xff) {

        /////////////////////////////////////////

        if (packet[1] == 0x01) {  // Button
          midimessage.type = 0;
          midimessage.note_pitch = key_note_map[packet[3] - 1];
          if (packet[2] == 0x01) {  // Button Pressed
            //Serial.printf("key %d pressed\n", packet[3] - 1);
            midimessage.note_state = 1;
          } else if (packet[2] == 0x02) {  // Button Released
            //Serial.printf("key %d released\n", packet[3] - 1);
            midimessage.note_state = 0;
          }
          message.type = midimessage.type;
          message.name = 0;
          message.value = midimessage.note_pitch;
        }

        ////////////////////////////////////////

        else if (packet[1] == 0xb2) {
          //if (current_mode == 0) {
          if (packet[2] == 0xca) {
            current_rotation = 1;
          } else if (packet[2] == 0xcb) {
            current_rotation = -1;
          }

          if (current_mode == 0) {
            if (current_rotation == 1) {
              midimessage.type = 1;
              if (encoder_state[packet[3] - 1] > 0) {
                encoder_state[packet[3] - 1] -= 5;
              }
              //Serial.printf("encoder:%d\n", encoder_state[packet[3] - 1]);
              midimessage.note_pitch = encoder_state[packet[3] - 1];
            } else if (current_rotation == -1) {
              //midimessage.type = 1;
              if (encoder_state[packet[3] - 1] < 127) {
                encoder_state[packet[3] - 1] += 5;
              }
              //Serial.printf("encoder:%d\n", encoder_state[packet[3] - 1]);
              midimessage.note_pitch = encoder_state[packet[3] - 1];
            }
            message.type = midimessage.type;
            message.name = 0;
            message.value = midimessage.note_pitch;
          } else if (current_mode == 1) {
            if (current_rotation == 1) {
              switch (packet[3]) {
                case 4:
                  if(pad_page_config <3) pad_page_config++;
                  break;
                case 3:
                  if(knob_page_config <3) knob_page_config++;
                  break;
                case 1:
                  trackctrl_direct = 1;
                  break;
              }
              //Serial.println(knob_page_config);
            } else if (current_rotation == -1) {
              switch (packet[3]) {
                case 4:
                  if(pad_page_config > 0) pad_page_config--;
                  break;
                case 3:
                  if(knob_page_config > 0) knob_page_config--;
                  break;
                case 1:
                  trackctrl_direct = -1;
                  break;
              }
            }
          }

        }

        else if (packet[1] == 0x03) {  // Button
          if (packet[3] == 0xa1) {
            current_mode = 1;
            if (packet[2] == 0x05) {
              play_pressed_time = millis();
              message.type = 2;
              message.name = 0;
              message.value = 0;
            } else if (packet[2] == 0x06) {
              stop_pressed_time = millis();
              message.type = 2;
              message.name = 0;
              message.value = 1;
            } else if (packet[2] == 0x07) {
              message.type = 2;
              message.name = 0;
              message.value = 2;
            }
            else if (packet[2] == 0x08) {
              //菜单
              menu_button_pressed_time = millis();
            }
            else if (packet[2] == 0x04) {
              message.type = 3;
              message.name = 0;
              message.value = 0;
            }

            else if (packet[2] == 0x03) {
              message.type = 3;
              message.name = 0;
              message.value = 1;
            } else if (packet[2] == 0x02) {
              message.type = 3;
              message.name = 0;
              message.value = 3;
            } else if (packet[2] == 0x01) {
              message.type = 3;
              message.name = 0;
              message.value = 2;
            }
          } 
          else if (packet[3] == 0xa2) {
            current_mode = 0;
            menu_button_pressed_time = 0;
            ban_trans = 1;
          }
        }



        if (QueueHandle_usb != NULL && uxQueueSpacesAvailable(QueueHandle_usb) > 0) {

          int ret = xQueueSend(QueueHandle_usb, (void *)&midimessage, 0);
          if (ret == pdTRUE) {
          } else if (ret == errQUEUE_FULL) {
            Serial.println("The `TaskReadFromSerial` was unable to send data into the Queue");
          }
        }
        if (QueueHandle_oled != NULL && uxQueueSpacesAvailable(QueueHandle_oled) > 0) {

          //sprintf(message.message, "Value:%d", midimessage.note_pitch);
          if(!ban_trans){

          int ret = xQueueSend(QueueHandle_oled, (void *)&message, 0);
          if (ret == pdTRUE) {
          } else if (ret == errQUEUE_FULL) {
            Serial.println("The `TaskReadFromSerial` was unable to send data into the Queue");
          }

          }
        }
      }
    }
    packet_idx = 0;
    packet_en = 0;
    current_rotation = 0;
    ban_trans = 0;
  }
}

typedef struct {
  char number;
  char sta;
} key_num_t;

typedef struct {
  char number;
  char rotation;
} knob_num_t;


void TaskWriteToMidi(void *pvParameters) {
  midi_message_t midimessage;
  while (1) {
    //bool ble_ok = BLEMidiServer.isConnected();
    if (QueueHandle_usb != NULL) {
      int ret = xQueueReceive(QueueHandle_usb, &midimessage, portMAX_DELAY);
      if (ret == pdPASS) {
        // The message was successfully received - send it back to Serial port and "Echo: "
        //Serial.printf("receive!");
        if (midimessage.type == 0) {
          if (midimessage.note_state == 1) {
#ifdef DEBUG
            MIDI.sendNoteOn(midimessage.note_pitch, 127, 1);
            if (ble_ok) {
              //BLEMidiServer.noteOn(0, midimessage.note_pitch, 127);
            }
#else
            Serial.printf("noteon:%d", midimessage.note_pitch);
#endif
          } else if (midimessage.note_state == 0) {
#ifdef DEBUG
            MIDI.sendNoteOff(midimessage.note_pitch, 0, 1);
            if (ble_ok) {
              //BLEMidiServer.noteOff(0, midimessage.note_pitch, 127);
            }
#else
            Serial.printf("noteoff:%d", midimessage.note_pitch);
#endif
          }
        } else if (midimessage.type == 1) {
#ifdef DEBUG
          MIDI.sendControlChange(42, midimessage.note_pitch, 12);
#else
          //Serial.printf("ccnote:%d", midimessage.note_pitch);
#endif
        }
      } else if (ret == pdFALSE) {
        Serial.println("The `TaskWriteToMIDIUSB` was unable to receive data from the Queue");
      }
    }
  }
}

void TaskFastLED(void *pvParameters) {
  while (1) {
    if (fadeDirection == 1) {  //fade up
      EVERY_N_MILLISECONDS(3) {
        fill_solid(leds, NUM_LEDS, CHSV(hue, sat, val));
        val = val + 1;
        if (val == 255) {
          fadeDirection = !fadeDirection;  //reverse direction
        }
      }
    }

    if (fadeDirection == 0) {  //fade down
      EVERY_N_MILLISECONDS(9) {
        fill_solid(leds, NUM_LEDS, CHSV(hue, sat, val));
        val = val - 1;
        if (val == 0) {
          fadeDirection = !fadeDirection;  //reverse direction
        }
      }
    }

    FastLED.show();

    EVERY_N_MILLISECONDS(90) {  //cycle around the color wheel over time
      hue++;
    }
  }
}


void ui_show(int type, int value1, int value2) {

  String temp_value;

  switch (type) {
    case 0:  // key
      u8g2.drawStr(0, 25, "KEY");
      temp_value = String("Value:") + String(value2);
      u8g2.drawStr(0, 40, temp_value.c_str());
      break;
    case 1:  // knob
      u8g2.drawStr(0, 25, "KNOB");
      temp_value = String("Value:") + String(value2);
      u8g2.drawStr(0, 40, temp_value.c_str());
      u8g2.drawFrame(1, 42, 127, 10);
      u8g2.drawBox(1, 42, value2, 10);
      break;
    case 2:  // button
      switch (value2) {
        case 0:
          u8g2.drawStr(0, 25, "PLAY");
          u8g2.drawStr(0, 40, temp_value.c_str());
          break;
        case 1:
          u8g2.drawStr(0, 25, "STOP");
          u8g2.drawStr(0, 40, temp_value.c_str());
          break;
        case 2:
          u8g2.drawStr(0, 25, "RECORD");
          u8g2.drawStr(0, 40, temp_value.c_str());
          break;
      }
      // u8g2.drawStr(0, 25, "BUT");
      // temp_value = String("Value:") + String(value2);
      // u8g2.drawStr(0, 40, temp_value.c_str());
      break;
    case 3:  //setting page or knob num,colour
      switch (value2) {
        case 0:
          u8g2.drawStr(0, 25, "KEY PAGE");
          temp_value = String("Number:") + String(pad_page_config);
          u8g2.drawStr(0, 40, temp_value.c_str());
          break;
        case 1:
          u8g2.drawStr(0, 25, "KNOB PAGE");
          temp_value = String("Number:") + String(knob_page_config);
          u8g2.drawStr(0, 40, temp_value.c_str());
          break;
        case 2:
          u8g2.drawStr(0, 25, "TRACKCTRL");
          if (trackctrl_direct == 1) {
            temp_value = String("rotation:") + ">>>";
          } else if (trackctrl_direct == -1) {
            temp_value = String("rotation:") + "<<<";
          }

          u8g2.drawStr(0, 40, temp_value.c_str());
          break;
        case 3:
          u8g2.drawStr(0, 25, "Light Mode");
          temp_value = String("Mode:") + String(light_mode_config_string[current_light_mode]);
          u8g2.drawStr(0, 40, temp_value.c_str());
          break;
      }
      break;
    default:
      break;
  }
}


#define UI_OVERTIME 1000

void TaskOLED(void *pvParameters) {
  char *helloword_string = "Ripi hELLO";
  char *showstring = NULL;
  message_t show_message;

  unsigned long last_event_time = 0;
  bool ui_flag;

  while (1) {
    u8g2.clearBuffer();
    String temp_value;

    if (QueueHandle_oled != NULL) {
      int ret = xQueueReceive(QueueHandle_oled, &show_message, 100);
      if (ret == pdPASS) {
        //helloword_string = "Ripi is not good.";
        //helloword_string = show_message.message;
        // u8g2.drawStr(0, 25, "KEY");
        //temp_value = String("Value:") + String(show_message.value);
        //u8g2.drawStr(40, 50, temp_value.c_str());
        //helloword_string = temp_value.;
        ui_flag = true;
        last_event_time = millis();
      } else {
      }
    }

    if (ui_flag) {
      ui_show(show_message.type, show_message.name, show_message.value);
      unsigned long temp = millis();
      if (temp - last_event_time > UI_OVERTIME) {
        ui_flag = 0;
        helloword_string = "Ripi is really good.";
      }
    }

    if (!ui_flag) {
      u8g2.setCursor(0, 15);
      u8g2.print(helloword_string);
    }

    u8g2.sendBuffer();
  }
}

// Declare a function type for menu actions.
typedef void (*menuAction)();
typedef String SubMenuItem;

struct MenuItem {
  String func_name;     // The name of the function to be displayed
  menuAction func_ptr;  // Function pointer for the function to be executed
};

void doSomething() {}
void doSomethingElse() {}

MenuItem menu_items[] = {
  { "Open OTA setting", &doSomething },
  { "Reload Config File", &doSomethingElse },
  { "Wireless OTA Update", &doSomething },
  { "KeyMode:", &doSomethingElse },
  { "LightMode:", &doSomethingElse },
};

SubMenuItem SubMenu_Mode_Items[] = {
  "Keyboard",
  "Midi",
  "Special"
};

SubMenuItem SubMenu_Light_Items[] = {
  "fluid",
  "shine",
  "nolight"
};



void task_menu(void *pvParameters) {

  message_t show_message;
  int current_index = 0;

  while (1) {
    u8g2.clearBuffer();
    
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0, 32, menu_items[current_index].func_name.c_str());
    /*
    if (current_index == 3) {
      u8g2.drawStr(0, 48, SubMenu_Mode_Items[key_modeconfig].c_str());
    } 
    else if (current_index == 4) {
      u8g2.drawStr(0, 48, SubMenu_Light_Items[light_modeconfig].c_str());
    }
    */

    if (QueueHandle_oled != NULL) {
      int ret = xQueueReceive(QueueHandle_oled, &show_message, 100);
      if (ret == pdPASS) {
        if(show_message.type == 2){
        //按键A按下
        if(show_message.value == 0){
          if (current_index < 4) current_index++; else current_index = 0;
        }
        //按键B按下
        else if(show_message.value == 1){
          if (current_index > 0) current_index--; else current_index = 4;
        }
        //按键C按下，执行
        //---------------------------------------------------------------//
        else if(show_message.value == 2){
          Serial.println("exect prog!");
          exec_setting_program(current_index);
          vTaskDelete(NULL);//delete self
        }
        }
      } 
      else {

      }
    }
    u8g2.sendBuffer();
    delay(2);
  }
}

void exec_expand_progrm(int program_number){
  vTaskDelete(LightTask);
  //vTaskDelete(TimtraceTask);
  vTaskDelete(taskMidi);
  vTaskDelete(MainProgressTask_OLED);
    switch(program_number){

    case 0:
      //模仿4页盒子主程序
      break;
    
    case 1:
      //音序器主程序
      break;

    case 2:
      break;
  }
}

void exec_setting_program(int setting_program){ 
  
  //vTaskDelete(TimtraceTask);
  //vTaskSuspend(MenuProgressTask_OLED);

  vTaskDelete(LightTask);
  //vTaskDelete(TimtraceTask);
  vTaskDelete(taskMidi);
  vTaskDelete(MainProgressTask_OLED);
  
  
  switch(setting_program){

    case 0:
      xTaskCreate(
      setting_program_void, "OpenSetup..."  // A name just for humans
      ,
      2048  // The stack size can be checked by calling `uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);`
      ,
      (void *)NULL,
      2  // Priority
      ,
      NULL  // Task handle is not used here - simply pass NULL
    );
      break;
    
    case 1:
      xTaskCreate(
      setting_program_void, "ReloadConfig..."  // A name just for humans
      ,
      2048  // The stack size can be checked by calling `uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);`
      ,
      (void *)NULL,
      2  // Priority
      ,
      NULL  // Task handle is not used here - simply pass NULL
      );
      break;

    case 2:
      xTaskCreate(
      setting_program_void, "WirelessOTA..."  // A name just for humans
      ,
      20480  // The stack size can be checked by calling `uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);`
      ,
      (void *)NULL,
      2  // Priority
      ,
      NULL  // Task handle is not used here - simply pass NULL
      );
      break;
  }
  /*
  xTaskCreate(
    setting_program, "configing..."  // A name just for humans
    ,
    2048  // The stack size can be checked by calling `uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);`
    ,
    (void *)NULL,
    2  // Priority
    ,
    NULL  // Task handle is not used here - simply pass NULL
  );
  */

}

char* wifi_ssid = "pig-wifi";
char* wifi_passwd = "xiebaobao";
int update_sta = 0;

void setting_program_void(void *pvParameters){
  String ui_temp_string;
  bool can_exec = false;

  for(int i=0;i<3;i++){
    u8g2.clearBuffer();
    ui_temp_string = String("begin in")+String(3-i)+String("sec...");
    u8g2.drawStr(0, 32, "Open WIFI for config...");
    u8g2.drawStr(0, 48, ui_temp_string.c_str());
    u8g2.sendBuffer();
    delay(1000);
  }

  //begin WIFI update
  while(1){
    message_t show_message;
    u8g2.clearBuffer();
    u8g2.drawStr(0, 16, "Continue?");
    ui_temp_string = String("wifi_ssid:")+String(wifi_ssid);
    u8g2.drawStr(0, 32, ui_temp_string.c_str());
    ui_temp_string = String("wifi_passwd:")+String(wifi_passwd);
    u8g2.drawStr(0, 48, ui_temp_string.c_str());
    u8g2.sendBuffer();
    if (QueueHandle_oled != NULL) {
      int ret = xQueueReceive(QueueHandle_oled, &show_message, 100);
      if (ret == pdPASS) {
        if(show_message.value == 2){
          can_exec = true;
          break;
        }
        }
    }
  }

  update_sta = 1;
  vTaskDelete(TimtraceTask);
  vTaskDelete(NULL);

}

void ota_update_void(void *pvParameters){
  while(1){
    u8g2.clearBuffer();
    u8g2.drawStr(0, 32, "Open WIFI for config...");
    u8g2.sendBuffer();
  }
}

#include <Update.h>
WiFiClient client;

long contentLength = 0;
bool isValidContentType = false;

String host = "bucket-name.s3.ap-south-1.amazonaws.com"; // Host => bucket-name.s3.region.amazonaws.com
int port = 80; // Non https. For HTTPS 443. As of today, HTTPS doesn't work.
String bin = "/sketch-name.ino.bin"; // bin file name with a slash in front.

void OTA_MAIN_PROG();

void loop() {
  if(update_sta == 1){
    WiFi.begin(wifi_ssid, wifi_passwd);
    while (WiFi.status() != WL_CONNECTED) {
        u8g2.clearBuffer();
        u8g2.drawStr(0, 32,"Connect to wifi");
        u8g2.sendBuffer();
        delay(1000);
        u8g2.clearBuffer();
        u8g2.drawStr(0, 32,"Connect to wifi.");
        u8g2.sendBuffer();
        delay(1000);
        u8g2.clearBuffer();
        u8g2.drawStr(0, 32,"Connect to wifi..");
        u8g2.sendBuffer();
        delay(1000);
        u8g2.clearBuffer();
        u8g2.drawStr(0, 32,"Connect to wifi...");
        u8g2.sendBuffer();
        delay(1000);
    }

    u8g2.clearBuffer();
    u8g2.drawStr(0, 32,"Connected...");
    u8g2.drawStr(0, 48,"Starting OTA...");
    u8g2.sendBuffer();

    delay(3000);

    OTA_MAIN_PROG();

    while(1){};
  }

}

String getHeaderValue(String header, String headerName) {
  return header.substring(strlen(headerName.c_str()));
}

void OTA_MAIN_PROG() {
  Serial.println("Connecting to: " + String(host));
  // Connect to S3
  if (client.connect(host.c_str(), port)) {
    // Connection Succeed.
    // Fecthing the bin
    Serial.println("Fetching Bin: " + String(bin));

    // Get the contents of the bin file
    client.print(String("GET ") + bin + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Cache-Control: no-cache\r\n" +
                 "Connection: close\r\n\r\n");

    // Check what is being sent
    //    Serial.print(String("GET ") + bin + " HTTP/1.1\r\n" +
    //                 "Host: " + host + "\r\n" +
    //                 "Cache-Control: no-cache\r\n" +
    //                 "Connection: close\r\n\r\n");

    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println("Client Timeout !");
        client.stop();
        return;
      }
    }
    // Once the response is available,
    // check stuff

    /*
       Response Structure
        HTTP/1.1 200 OK
        x-amz-id-2: NVKxnU1aIQMmpGKhSwpCBh8y2JPbak18QLIfE+OiUDOos+7UftZKjtCFqrwsGOZRN5Zee0jpTd0=
        x-amz-request-id: 2D56B47560B764EC
        Date: Wed, 14 Jun 2017 03:33:59 GMT
        Last-Modified: Fri, 02 Jun 2017 14:50:11 GMT
        ETag: "d2afebbaaebc38cd669ce36727152af9"
        Accept-Ranges: bytes
        Content-Type: application/octet-stream
        Content-Length: 357280
        Server: AmazonS3
                                   
        {{BIN FILE CONTENTS}}

    */
    while (client.available()) {
      // read line till /n
      String line = client.readStringUntil('\n');
      // remove space, to check if the line is end of headers
      line.trim();

      // if the the line is empty,
      // this is end of headers
      // break the while and feed the
      // remaining `client` to the
      // Update.writeStream();
      if (!line.length()) {
        //headers ended
        break; // and get the OTA started
      }

      // Check if the HTTP Response is 200
      // else break and Exit Update
      if (line.startsWith("HTTP/1.1")) {
        if (line.indexOf("200") < 0) {
          Serial.println("Got a non 200 status code from server. Exiting OTA Update.");
          break;
        }
      }

      // extract headers here
      // Start with content length
      if (line.startsWith("Content-Length: ")) {
        contentLength = atol((getHeaderValue(line, "Content-Length: ")).c_str());
        Serial.println("Got " + String(contentLength) + " bytes from server");
      }

      // Next, the content type
      if (line.startsWith("Content-Type: ")) {
        String contentType = getHeaderValue(line, "Content-Type: ");
        Serial.println("Got " + contentType + " payload.");
        if (contentType == "application/octet-stream") {
          isValidContentType = true;
        }
      }
    }
  } else {
    // Connect to S3 failed
    // May be try?
    // Probably a choppy network?
    Serial.println("Connection to " + String(host) + " failed. Please check your setup");
    // retry??
    // execOTA();
  }

  // Check what is the contentLength and if content type is `application/octet-stream`
  Serial.println("contentLength : " + String(contentLength) + ", isValidContentType : " + String(isValidContentType));

  // check contentLength and content type
  if (contentLength && isValidContentType) {
    // Check if there is enough to OTA Update
    bool canBegin = Update.begin(contentLength);

    // If yes, begin
    if (canBegin) {
      Serial.println("Begin OTA. This may take 2 - 5 mins to complete. Things might be quite for a while.. Patience!");
      // No activity would appear on the Serial monitor
      // So be patient. This may take 2 - 5mins to complete
      size_t written = Update.writeStream(client);

      if (written == contentLength) {
        Serial.println("Written : " + String(written) + " successfully");
      } else {
        Serial.println("Written only : " + String(written) + "/" + String(contentLength) + ". Retry?" );
        // retry??
        // execOTA();
      }

      if (Update.end()) {
        Serial.println("OTA done!");
        if (Update.isFinished()) {
          Serial.println("Update successfully completed. Rebooting.");
          ESP.restart();
        } else {
          Serial.println("Update not finished? Something went wrong!");
        }
      } else {
        Serial.println("Error Occurred. Error #: " + String(Update.getError()));
      }
    } else {
      // not enough space to begin OTA
      // Understand the partitions and
      // space availability
      Serial.println("Not enough space to begin OTA");
      client.flush();
    }
  } else {
    Serial.println("There was no content in the response");
    client.flush();
  }
}