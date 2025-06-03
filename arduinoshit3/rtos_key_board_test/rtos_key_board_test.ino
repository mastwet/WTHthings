#include <ShiftIn.h>

#include <U8g2lib.h>
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE, /* clock=*/18, /* data=*/17);

QueueHandle_t QueueHandle_usb;
const int QueueElementSize = 10;

ShiftIn<3> shift_key;
ShiftIn<1> shift_button;
ShiftIn<1> shift_encoder;

// pins for 74HC165
#define KEYPAD_PIN_DATA 14  // pin 9 on 74HC165 (DATA)
#define KEYPAD_PIN_LOAD 13  // pin 1 on 74HC165 (LOAD)
#define KEYPAD_PIN_CLK 12   // pin 2 on 74HC165 (CLK))

#define ENCODER_PIN_DATA 11  // pin 9 on 74HC165 (DATA)
#define ENCODER_PIN_LOAD 21  // pin 1 on 74HC165 (LOAD)
#define ENCODER_PIN_CLK 47   // pin 2 on 74HC165 (CLK))

#define BUTTON_PIN_DATA 35  // pin 9 on 74HC165 (DATA)
#define BUTTON_PIN_LOAD 36  // pin 1 on 74HC165 (LOAD)
#define BUTTON_PIN_CLK 37   // pin 2 on 74HC165 (CLK))

bool pinState[8];
bool key_pinState[8];

bool data2[8];
unsigned int encoder_state[4] = { 0 };

typedef struct {
  char name[10];
  char message[100];
} message_t;

//type 0:Note
//type 1:ControlChange

typedef struct {
  char type;
  char note_pitch;
  uint8_t note_state;
} midi_message_t;

typedef struct
{
  bool val;
  bool last_val;
} KEY;

KEY key[10] = { false };

void encoder_midi_queue_send(char encoder_num, char direction);
void TaskWriteToMidi(void *pvParameters);
void TaskReadEncoder(void *pvParameters);
void ReadKeypad(void *pvParameters);
void TaskReadKey(void *pvParameters);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  shift_encoder.begin(ENCODER_PIN_LOAD, 9, ENCODER_PIN_DATA, ENCODER_PIN_CLK);
  shift_key.begin(KEYPAD_PIN_LOAD, 9, KEYPAD_PIN_DATA, KEYPAD_PIN_CLK);
  shift_button.begin(BUTTON_PIN_LOAD, 9, BUTTON_PIN_DATA, BUTTON_PIN_CLK);
  
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setCursor(0, 15);
  u8g2.print("FUCKYOU WORLD!");
  u8g2.sendBuffer();

  pinMode(3, INPUT);
  pinMode(48, INPUT);
  pinMode(45, INPUT);

  QueueHandle_usb = xQueueCreate(QueueElementSize, sizeof(midi_message_t));
  //QueueHandle_oled = xQueueCreate(QueueElementSize, sizeof(message_t));
  // Check if the queue was successfully created
  if (QueueHandle_usb == NULL /* || QueueHandle_oled == NULL*/) {
    while (1) {
      Serial.println("Queue could not be created. Halt.");
      delay(1000);  // Halt at this point as is not possible to continue
    }
  } else {
    Serial.println("Queue create sucessful.");
  }

  xTaskCreate(
    TaskReadEncoder, "Encoder Task"  // A name just for humans
    ,
    2048  // The stack size can be checked by calling `uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);`
    ,
    (void *)NULL,
    2  // Priority
    ,
    NULL  // Task handle is not used here - simply pass NULL
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
    NULL  // Task handle is not used here
  );

  xTaskCreate(
    ReadKeypad, "Read Keypad"  // A name just for humans
    ,
    2048  // The stack size can be checked by calling `uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);`
    ,
    (void *)NULL,
    2  // Priority
    ,
    NULL  // Task handle is not used here - simply pass NULL
  );

  xTaskCreate(
    TaskReadKey, "Read Button"  // A name just for humans
    ,
    2048  // The stack size can be checked by calling `uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);`
    ,
    (void *)NULL,
    2  // Priority
    ,
    NULL  // Task handle is not used here - simply pass NULL
  );
}

void TaskReadKey(void *pvParameters) {
  while (1) {
    shift_button.update();
    for (int i = 0; i < 10; i++) {
      if (i < 8) {
        key[i].val = shift_button.state(i);
      } else if (i == 8) {
        key[i].val = digitalRead(48);
      } else if (i == 9) {
        key[i].val = digitalRead(45);
      }
      // if time is greater than debounce delay
      //button_print(i);

      if (key[i].last_val == 1 && key[i].val == 0) {
        // send midi message to queue
        Serial.printf("key%dpressed\r\n", i);
        delay(200);
      }

      key[i].last_val = key[i].val;
      delay(2);
    }
  }
}

void TaskReadEncoder(void *pvParameters) {
  int pre_state = 0;
  bool data[8];

  while (1) {
    shift_encoder.update();
    for (int i = 0; i < 8; i++) {
      data[i] = shift_encoder.state(i);
      if (data[i] != pinState[i]) {
        if (pinState[0] == 1 && data[0] == 0) {
          //falling edge
          if (data[1] == HIGH) {
            encoder_midi_queue_send(0, 0);
          }
          if (data[1] == LOW) {
            encoder_midi_queue_send(0, 1);
          }
        }
        if (pinState[2] == 1 && data[2] == 0) {
          if (data[3] == HIGH) {
            encoder_midi_queue_send(1, 1);
          }
          if (data[3] == LOW) {
            encoder_midi_queue_send(1, 0);
          }
        }
        if (pinState[4] == 1 && data[4] == 0) {
          //falling edge
          if (data[5] == HIGH) {
            encoder_midi_queue_send(2, 1);
          }
          if (data[5] == LOW) {
            encoder_midi_queue_send(2, 0);
          }
        }
        if (pinState[6] == 1 && data[6] == 0) {
          //falling edge
          if (data[7] == HIGH) {
            encoder_midi_queue_send(3, 1);
          }
          if (data[7] == LOW) {
            encoder_midi_queue_send(3, 0);
          }
        }
      }
      pinState[i] = data[i];
    }
    delay(1);
  }
}


void ReadKeypad(void *pvParameters) {  // This is a task.
  // put your main code here, to run repeatedly:
  midi_message_t midimessage;
  bool pinState_key[25] = { 0 };
  int remap[25] = { 16, 17, 18, 19, 20, 21, 22, 23, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7, 24 };

  while (1) {
    shift_key.update();
    bool data;
    for (uint8_t i = 0, n = 25; i < n; i++) {
      if (i < 24) {
        data = shift_key.state(i);
      } else {
        data = digitalRead(3);
      }
      if (data != pinState_key[i]) {
        midimessage.type = 0;
        midimessage.note_pitch = remap[i];
        if (data) {
          midimessage.note_state = 1;
        } else {
          midimessage.note_state = 0;
        }
        if (QueueHandle_usb != NULL && uxQueueSpacesAvailable(QueueHandle_usb) > 0) {

          int ret = xQueueSend(QueueHandle_usb, (void *)&midimessage, 0);
          if (ret == pdTRUE) {
          } else if (ret == errQUEUE_FULL) {
            Serial.println("The `TaskReadFromSerial` was unable to send data into the Queue");
          }
        }
        pinState_key[i] = data;
        // Serial.print("Pin ");
        // Serial.print(remap[i]);
        // Serial.print(": ");
        // Serial.println(data);
      }
    }
    // bool data = digitalRead(3);
    // key_print(data, 24);
    //Serial.print((char)e.bit.KEY);
    // if (e.bit.EVENT == KEY_JUST_PRESSED) Serial.println(" pressed");
    // else if (e.bit.EVENT == KEY_JUST_RELEASED) Serial.println(" released");
    delay(2);
  }
}

void encoder_midi_queue_send(char encoder_num, char direction) {

  if (direction == 1) {
    if (encoder_state[encoder_num] < 127) {
      encoder_state[encoder_num] += 1;
    }

  } else if (direction == 0) {
    if (encoder_state[encoder_num] > 1) {
      encoder_state[encoder_num] -= 1;
    }
  }
  if (QueueHandle_usb != NULL && uxQueueSpacesAvailable(QueueHandle_usb) > 0) {
    midi_message_t midi_message;
    midi_message.type = 1;
    midi_message.note_pitch = encoder_state[encoder_num];
    int ret = xQueueSend(QueueHandle_usb, (void *)&midi_message, 0);
    if (ret == pdTRUE) {
    } else if (ret == errQUEUE_FULL) {
      Serial.println("The `TaskReadFromSerial` was unable to send data into the Queue");
    }
  }
}

void TaskWriteToMidi(void *pvParameters) {
  midi_message_t midimessage;
  while (1) {
    if (QueueHandle_usb != NULL) {
      int ret = xQueueReceive(QueueHandle_usb, &midimessage, portMAX_DELAY);
      if (ret == pdPASS) {
        // The message was successfully received - send it back to Serial port and "Echo: "
        //Serial.printf("receive!");
        if (midimessage.type == 0) {
          if (midimessage.note_state == 1) {
            //midi note on
            Serial.printf("midinoteon:%d \r\n", midimessage.note_pitch);
            //MIDI.sendNoteOn(midimessage.note_pitch, 127, 1);
          } else if (midimessage.note_state == 0) {
            //midi off
            Serial.printf("midinoteoff:%d \r\n", midimessage.note_pitch);
            //MIDI.sendNoteOff(midimessage.note_pitch, 0, 1);
          }
        } else if (midimessage.type == 1) {
          //midi cc
          Serial.printf("midicc:%d \r\n", midimessage.note_pitch);
          //MIDI.sendControlChange(42, midimessage.note_pitch, 12);
        }
      } else if (ret == pdFALSE) {
        Serial.println("The `TaskWriteToMIDIUSB` was unable to receive data from the Queue");
      }
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
}
