#include <U8g2lib.h>
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE, /* clock=*/4, /* data=*/2);

#include <FastLED.h>
#define LED_PIN 11
#define NUM_LEDS 16
CRGB leds[NUM_LEDS];

typedef struct {
  char number;
  char sta;
} key_num_t;

typedef struct {
  char number;
  char rotation;
} knob_num_t;

typedef struct {
  char type;
  char number;
  char message;
} h_message_t;

QueueHandle_t QueueHandle_h_message;
const int QueueElementSize = 10;


void tinytt_send_message(h_message_t message) {
  if (QueueHandle_h_message != NULL && uxQueueSpacesAvailable(QueueHandle_h_message) > 0) {
    int ret = xQueueSend(QueueHandle_h_message, (void *)&message, 0);
    if (ret == pdTRUE) {
    } else if (ret == errQUEUE_FULL) {
      Serial.println("The `tinytt_send_message` was unable to send data into the Queue");
    }
  }
}

//更小的光溯读取器
void ReadFromTimtraceTiny(void *pvParameters) {
  // put your main code here, to run repeatedly:
  byte packet[30] = { 0 };
  byte encoder_state[4] = { 0 };

  int packet_idx = 0;
  int packet_en = 1;
  int ban_trans = 0;


  while (1) {
    delay(10);
    //主程序
    while (Serial1.available()) {

      //串口阻塞读取程序外判断，是否菜单按下时时间达到标准。如果达到标准执行任务调度。
      byte data = Serial1.read();
      packet[packet_idx++] = data;

      if (data == 0xff) {
        h_message_t h_message;
        /////////////////////////////////////////

        if (packet[1] == 0x01) {  // Button
          if (packet[2] == 0x01) {  // Button Pressed
            //Serial.printf("key %d pressed\n", packet[3]);
            h_message.type = 0;
            h_message.number = packet[3];
            h_message.message = 1;
          } else if (packet[2] == 0x02) {  // Button Released
            //Serial.printf("key %d released\n", packet[3]);
            h_message.type = 0;
            h_message.number = packet[3];
            h_message.message = 0;
          }
        }

        ////////////////////////////////////////
        //knob
        else if (packet[1] == 0xb2) {
          //if (current_mode == 0) {
          if (packet[2] == 0xca) {
            //Serial.printf("knob %d left\n", packet[3] - 1);
            h_message.type = 1;
            h_message.number = packet[3];
            h_message.message = 1;
            //current_rotation = 1;
          } else if (packet[2] == 0xcb) {
            //Serial.printf("knob %d right\n", packet[3] - 1);
            h_message.type = 1;
            h_message.number = packet[3];
            h_message.message = 0;
            //current_rotation = -1;
          }

        }

        else if (packet[1] == 0x03) {  // Button
          if (packet[3] == 0xa1) {
            //current_mode = 1;
            h_message.type = 2;
            h_message.number = packet[2];
            h_message.message = 0;
            /*
            if (packet[2] == 0x05) {
              Serial.printf("key 1 pressed\n");
            } else if (packet[2] == 0x06) {
              Serial.printf("key 2 pressed\n");
            } else if (packet[2] == 0x07) {
              Serial.printf("key 3 pressed\n");
            } else if (packet[2] == 0x08) {
              Serial.printf("key 4 pressed\n");
            } else if (packet[2] == 0x04) {
              Serial.printf("key 5 pressed\n");
            } else if (packet[2] == 0x03) {
              Serial.printf("key 6 pressed\n");
            } else if (packet[2] == 0x02) {
              Serial.printf("key 7 pressed\n");
            } else if (packet[2] == 0x01) {
              Serial.printf("key 8 pressed\n");
            }
          */
          }
          
          //按下
          else if (packet[3] == 0xa2) {
            h_message.type = 2;
            h_message.number = packet[2];
            h_message.message = 1;
          }
        }

        if (QueueHandle_h_message != NULL && uxQueueSpacesAvailable(QueueHandle_h_message) > 0) {

          //sprintf(message.message, "Value:%d", midimessage.note_pitch);

          int ret = xQueueSend(QueueHandle_h_message, (void *)&h_message, 0);
          if (ret == pdTRUE) {
          } else if (ret == errQUEUE_FULL) {
            Serial.println("unable to send data into the Queue");
          }
          
        }

        packet_idx = 0;
      }
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, 6, 5);

  QueueHandle_h_message = xQueueCreate(QueueElementSize, sizeof(h_message_t));
  // Check if the queue was successfully created
  if (QueueHandle_h_message == NULL || QueueHandle_h_message == NULL) {
    while (1) {
      Serial.println("Queue could not be created. Halt.");
      delay(1000);  // Halt at this point as is not possible to continue
    }
  } else {
    Serial.println("Queue create sucessful.");
  }

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);

  u8g2.setBusClock(800000);
  u8g2.begin();
  u8g2.setFont(u8g2_font_ncenB08_tr);

  xTaskCreate(
    ReadFromTimtraceTiny, "timtrace"  // A name just for humans
    ,
    2048  // The stack size can be checked by calling `uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);`
    ,
    (void *)NULL,
    2  // Priority
    ,
    NULL  // Task handle is not used here - simply pass NULL
  );

  //leds init
  for (int i = 0; i < 16; i++) {
    leds[i] = CRGB::Black;
  }
}

int mode = 0;  //几种模式：1、cc触发输出 2、音符触发输出 3、钢琴模式 4、随机音序输出模式

/*
enum key_sta_t {
  ON,//BLUE
  RISING,//GREEN
  OFF
};
*/

// typedef struct {
//   led sta;
// } led_sta[16];

struct key {
  int pre;
  int now;
};

key cc_on_keymap[16];
key note_on_keymap[16];


void drawLogo(void) {

  for (int j = 0; j < 4; j++) {
    for (int i = 0; i < 4; i++) {
      if(cc_on_keymap[j*4+i].now == 1){
          u8g2.drawBox(12 + 10 * i + 2 * i, 7 + 5 * j + 2 * j, 10, 5);
      }
      else{
          u8g2.drawFrame(12 + 10 * i + 2 * i, 7 + 5 * j + 2 * j, 10, 5);
      }
    }
  }

  for (int j = 0; j < 4; j++) {
    for (int i = 0; i < 4; i++) {
      u8g2.drawFrame(62 + 10 * i + 2 * i, 7 + 5 * j + 2 * j, 10, 5);
    }
  }

  for (int j = 0; j < 4; j++) {
    for (int i = 0; i < 4; i++) {
      u8g2.drawFrame(62 + 10 * i + 2 * i, 36 + 5 * j + 2 * j, 10, 5);
    }
  }

  for (int j = 0; j < 4; j++) {
    for (int i = 0; i < 4; i++) {
      u8g2.drawFrame(12 + 10 * i + 2 * i, 36 + 5 * j + 2 * j, 10, 5);
    }
  }


  /*
  for (int i = 0; i < 2; i++) {
    u8g2.drawBox(12 + 5 * i + 2 * i, 9, 5, 10);
  }
  for (int i = 0; i < 3; i++) {
    u8g2.drawBox(34 + 5 * i + 2 * i, 9, 5, 10);
  }

  for (int i = 0; i < 2; i++) {
    u8g2.drawBox(64 + 5 * i + 2 * i, 9, 5, 10);
  }

  for (int i = 0; i < 3; i++) {
    u8g2.drawBox(90 + 5 * i + 2 * i, 9, 5, 10);
  }

  for (int i = 0; i < 16; i++) {
    u8g2.drawFrame(5 + 5 * i + 2 * i, 20, 5, 10);
  }

  for (int i = 0; i < 2; i++) {
    u8g2.drawBox(12 + 5 * i + 2 * i, 39, 5, 10);
  }
  for (int i = 0; i < 3; i++) {
    u8g2.drawBox(34 + 5 * i + 2 * i, 39, 5, 10);
  }

  for (int i = 0; i < 2; i++) {
    u8g2.drawBox(64 + 5 * i + 2 * i, 39, 5, 10);
  }

  for (int i = 0; i < 3; i++) {
    u8g2.drawBox(90 + 5 * i + 2 * i, 39, 5, 10);
  }

  for (int i = 0; i < 16; i++) {
    u8g2.drawFrame(5 + 5 * i + 2 * i, 50, 5, 10);
  }
  */
}

int fpage_running_mode = 0;

void cc_trigger_mode(h_message_t h_message);
void note_trigger_mode();
void piano_mode();
void cut_mode();





int cc_trigger_current_page = 0;
int note_trigger_current_page = 0;
int piano_current_page = 0;

bool piano_light_map[16][4];

unsigned long cut_key_time_map[16];

void loop() {

  h_message_t h_message;
  int ret = xQueueReceive(QueueHandle_h_message, &h_message, portMAX_DELAY);
  if (ret == pdPASS) {
    Serial.println("received sucess!");
  } else if (ret == pdFALSE) {
    Serial.println("The `TaskWriteToMIDIUSB` was unable to receive data from the Queue");
  }

    switch (fpage_running_mode) {
        //cc触发模式
      case 0:
        cc_trigger_mode(h_message);
        break;

      // //note触发模式
      // case 1:
      //   note_trigger_mode();
      //   break;

      // //钢琴模式
      // case 2:
      //   piano_mode();
      //   break;

      // //连续切分模式
      // case 2:
      //   cut_mode();
      //   break;
    }



  u8g2.clearBuffer();
  //1、触发模式：显示四个页的实时样式
  drawLogo();
  u8g2.sendBuffer();

  /*
  for (int i = 0; i < 16; i++) {
    switch (led_sta[i]) {
      case TRIGGER_ON_BLUE:
        leds[i] = CRGB::Blue;
        break;
      case TRIGGER_RISING_ON_GREEN:
        leds[i] = CRGB::Green;
        break;
      case TRIGGER_ON_RED:
        leds[i] = CRGB::Red;
        break;
      case NOTE_ON_WHITE:
        leds[i] = CRGB::Black;
        break;
    }
    FastLED.show();
  }
  */
}

void writenote(int a, int b) {
  Serial.printf("%d,%d",a,b);
}

bool cc_trigger_pad[16] = {false};

void cc_trigger_mode(h_message_t h_message) {
    //Serial.printf("%d,%d,%d",h_message.number,h_message.type,h_message.message);
    cc_on_keymap[h_message.number-1].now = h_message.message;
    for (int i = 0; i < 16; i++) {
    if (cc_on_keymap[i].pre != cc_on_keymap[i].now) {
      if (cc_on_keymap[i].now == 1) {
        //按下
        if(cc_trigger_pad[i] == false){
          Serial.printf("%d trigger on",i);
          cc_trigger_pad[i] = true;
        }
        else{
          Serial.printf("%d trigger off",i);
          cc_trigger_pad[i] = false;
        }

        //writenote(i, 1);
      }
      //  else {
      //   writenote(i, 0);
      // }
    }
    // switch (cc_on_keymap[i].now) {
      switch ((int)cc_trigger_pad[i]) {
      case 0:
        leds[i] = CRGB::Black;
        break;
      case 1:
        leds[i] = CRGB::Blue;
        break;
    }
    FastLED.show();
  }
  cc_on_keymap[h_message.number-1].pre = h_message.message;
}
/*

void note_trigger_mode() {
    note_on_keymap[h_message.number].now = h_message.message;
    for (int i = 0; i < 16; i++) {
    if (note_on_keymap[i].pre != note_on_keymap[i].now) {
      if (note_on_keymap[i].now == 1) {
        writenote(i, 1);
      } else {
        writenote(i, 0);
      }
    }
    switch (note_on_keymap[i].now) {
      case 0:
        leds[i] = CRGB::Black;
        break;
      case 1:
        leds[i] = CRGB::Green;
        break;
    }
    FastLED.show();
  }
  note_on_keymap[h_message.number].pre = h_message.message;
}

void piano_mode(h_message_t *h_message) {
  for(int i=0;i<16;i++){
    switch (piano_light_map[i][piano_current_page]) {
      case 0:
        leds[i] = CRGB::Black;
        break;
      case 1:
        leds[i] = CRGB::White;
        break;
    }
  }

  if (h_message.message == 1) {
    writenote(h_message.number, 1);
  } else {
    writenote(h_message.number, 0);
  }

}

void cut_mode() {
}
*/
