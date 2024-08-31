#include <ShiftIn.h>

ShiftIn<1> shift_encoder_key;
ShiftIn<3> shift_key;

// pins for 74HC165
#define PIN_DATA 37 // pin 9 on 74HC165 (DATA)
#define PIN_LOAD 35 // pin 1 on 74HC165 (LOAD)
#define PIN_CLK 36  // pin 2 on 74HC165 (CLK))

#define PIN2_DATA 14 // pin 9 on 74HC165 (DATA)
#define PIN2_LOAD 13 // pin 1 on 74HC165 (LOAD)
#define PIN2_CLK 12  // pin 2 on 74HC165 (CLK))

byte normal_note_sequence[] = {
    48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72};

bool pinState[8];
bool key_pinState[8];

bool data2[8];

typedef struct
{
  bool val;
  bool last_val;
} KEY;

KEY key[8] = {false};

typedef struct
{
  uint8_t id;
  bool pressed;
} KEY_MSG;

KEY_MSG key_msg = {0};

#include <U8g2lib.h>
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE, /* clock=*/4, /* data=*/2); // ESP32 Thing, HW I2C with pin remapping

#include <FastLED.h>
#define LED_PIN 11
#define NUM_LEDS 16
CRGB leds[NUM_LEDS];

uint8_t hue = 0;
uint8_t sat = 255;
uint8_t val = 0;
boolean fadeDirection = 1; // [1=fade up, 0=fade down]

#include <MIDI.h>

// #include <BLEMidi.h>
/*
#include <Adafruit_TinyUSB.h>
// USB MIDI object
Adafruit_USBD_MIDI usb_midi;

// Create a new instance of the Arduino MIDI Library,
// and attach usb_midi as the transport.
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI);
*/

// Define Queue handle
QueueHandle_t QueueHandle_usb;
QueueHandle_t QueueHandle_oled;

const int QueueElementSize = 10;

typedef struct
{
  char name[10];
  char message[100];
} message_t;

// type 0:Note
// type 1:ControlChange

typedef struct
{
  char type;
  char note_pitch;
  uint8_t note_state;
} midi_message_t;

void ReadKeypad(void *pvParameters);
void TaskWriteToMidi(void *pvParameters);
void TaskFastLED(void *pvParameters);
void TaskOLED(void *pvParameters);
void TaskReadKeyAndEncoder(void *pvParameters);

void setup()
{
  Serial.begin(115200);
  // shift_encoder_key.begin(PIN_LOAD, 8, PIN_DATA, PIN_CLK);
  //shift_key.begin(PIN2_LOAD, 8, PIN2_DATA, PIN2_CLK);
  shift_key.begin(13, 9, 14, 12);

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);

  u8g2.setBusClock(800000);
  u8g2.begin();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.println("Hello World!");
  /*
  BLEMidiServer.begin("RapidPulze Oreoly");

#if defined(ARDUINO_ARCH_MBED) && defined(ARDUINO_ARCH_RP2040)
  // Manual begin() is required on core without built-in support for TinyUSB such as mbed rp2040
  TinyUSB_Device_Init(0);
#endif
  MIDI.begin(MIDI_CHANNEL_OMNI);

  while (!TinyUSBDevice.mounted())
  {
    Serial.println("Not OK!");
    delay(100);
  }

  QueueHandle_usb = xQueueCreate(QueueElementSize, sizeof(midi_message_t));
  QueueHandle_oled = xQueueCreate(QueueElementSize, sizeof(message_t));
  // Check if the queue was successfully created
  if (QueueHandle_usb == NULL || QueueHandle_oled == NULL)
  {
    while (1)
    {
      Serial.println("Queue could not be created. Halt.");
      delay(1000); // Halt at this point as is not possible to continue
    }
  }
  else
  {
    Serial.println("Queue create sucessful.");
  }
*/
  xTaskCreate(
      ReadKeypad, "Read Keypad" // A name just for humans
      ,
      2048 // The stack size can be checked by calling `uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);`
      ,
      (void *)NULL,
      2 // Priority
      ,
      NULL // Task handle is not used here - simply pass NULL
  );
  /*
    xTaskCreate(
        TaskWriteToMidi, "Task Write To Midi" // A name just for humans
        ,
        2048 // The stack size can be checked by calling `uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);`
        ,
        (void *)NULL // No parameter is used
        ,
        2 // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
        ,
        NULL // Task handle is not used here
    );

    xTaskCreate(
        TaskFastLED, "Task RGB Light" // A name just for humans
        ,
        2048 // The stack size can be checked by calling `uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);`
        ,
        (void *)NULL // No parameter is used
        ,
        2 // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
        ,
        NULL // Task handle is not used here
    );

    xTaskCreate(
        TaskOLED, "OLED Task" // A name just for humans
        ,
        2048 // The stack size can be checked by calling `uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);`
        ,
        (void *)NULL,
        2 // Priority
        ,
        NULL // Task handle is not used here - simply pass NULL
    );

    xTaskCreate(
        TaskReadKeyAndEncoder, "Encoder&key Task" // A name just for humans
        ,
        2048 // The stack size can be checked by calling `uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);`
        ,
        (void *)NULL,
        2 // Priority
        ,
        NULL // Task handle is not used here - simply pass NULL
    );
  */
}

unsigned int encoder_state[4] = {0};

void encoder_midi_queue_send(char encoder_num, char direction)
{

  if (direction == 1)
  {
    if (encoder_state[encoder_num] < 127)
    {
      encoder_state[encoder_num] += 1;
    }
  }
  else if (direction == 0)
  {
    if (encoder_state[encoder_num] > 1)
    {
      encoder_state[encoder_num] -= 1;
    }
  }
  /*
  if (QueueHandle_oled != NULL && uxQueueSpacesAvailable(QueueHandle_oled) > 0)
  {
    message_t message;
    sprintf(message.message, "Knob Value:%d", encoder_state[encoder_num]);
    int ret = xQueueSend(QueueHandle_oled, (void *)&message, 0);
    if (ret == pdTRUE)
    {
    }
    else if (ret == errQUEUE_FULL)
    {
      Serial.println("The `TaskReadFromSerial` was unable to send data into the Queue");
    }
  }

  if (QueueHandle_usb != NULL && uxQueueSpacesAvailable(QueueHandle_usb) > 0)
  {
    midi_message_t midi_message;
    midi_message.type = 1;
    midi_message.note_pitch = encoder_state[encoder_num];
    int ret = xQueueSend(QueueHandle_usb, (void *)&midi_message, 0);
    if (ret == pdTRUE)
    {
    }
    else if (ret == errQUEUE_FULL)
    {
      Serial.println("The `TaskReadFromSerial` was unable to send data into the Queue");
    }
  }
  */
  Serial.println(encoder_state[encoder_num]);
}

void key_queue_send(char key_num)
{
  if (QueueHandle_usb != NULL && uxQueueSpacesAvailable(QueueHandle_usb) > 0)
  {
    Serial.println(key_num);
    /*
    midi_message_t midi_message;
    midi_message.type = 0;
    midi_message.note_pitch = key_num;
    int ret = xQueueSend(QueueHandle_usb, (void *)&midi_message, 0);
    if (ret == pdTRUE)
    {
    }
    else if (ret == errQUEUE_FULL)
    {
      Serial.println("The `TaskReadFromSerial` was unable to send data into the Queue");
    }
    */
  }
}

void TaskReadKeyAndEncoder(void *pvParameters)
{
  int pre_state = 0;
  bool encoder_data[8];
  unsigned long lastDebounceTime = 0;
  unsigned long debounceDelay = 200;

  while (1)
  {
    shift_encoder_key.update();
    // read key value
    for (int i = 0; i < 8; i++)
    {
      key[i].val = shift_encoder_key.state(i);
      // if time is greater than debounce delay
      if (key[i].last_val == 1 && key[i].val == 0 && (millis() - lastDebounceTime > debounceDelay))
      {
        // send midi message to queue
        key_queue_send(i);
      }
      lastDebounceTime = millis();
      key[i].last_val = key[i].val;
    }
  }
  // read encoder value
  for (int i = 8; i < 16; i++)
  {
    encoder_data[i] = shift_encoder_key.state(i);
    if (encoder_data[i] != pinState[i])
    {
      if (pinState[0] == 1 && encoder_data[0] == 0)
      {
        // falling edge
        if (encoder_data[1] == HIGH)
        {
          encoder_midi_queue_send(0, 0);
        }
        if (encoder_data[1] == LOW)
        {
          encoder_midi_queue_send(0, 1);
        }
      }
      if (pinState[2] == 1 && encoder_data[2] == 0)
      {
        if (encoder_data[3] == HIGH)
        {
          encoder_midi_queue_send(1, 1);
        }
        if (encoder_data[3] == LOW)
        {
          encoder_midi_queue_send(1, 0);
        }
      }
      if (pinState[4] == 1 && encoder_data[4] == 0)
      {
        // falling edge
        if (encoder_data[5] == HIGH)
        {
          encoder_midi_queue_send(2, 1);
        }
        if (encoder_data[5] == LOW)
        {
          encoder_midi_queue_send(2, 0);
        }
      }
      if (pinState[6] == 1 && encoder_data[6] == 0)
      {
        // falling edge
        if (encoder_data[7] == HIGH)
        {
          encoder_midi_queue_send(3, 1);
        }
        if (encoder_data[7] == LOW)
        {
          encoder_midi_queue_send(3, 0);
        }
      }
    }
    pinState[i] = encoder_data[i];
  }

  delay(1);
}

void ReadKeypad(void *pvParameters)
{ // This is a task.
  // put your main code here, to run repeatedly:
  midi_message_t midimessage;
  bool pinState[24] = {false};
  while (1)
  {
    Serial.println("begin");
    for (uint8_t i = 0, n = shift_key.getDataWidth(); i < n; i++)
    {
      bool data = shift_key.state(i);
      if (data != pinState[i])
      {
        pinState[i] = data;
        midimessage.type = 0;
        if (pinState[i] == true)
        {
          // MIDI.sendNoteOn(normal_note_sequence[i], 127, 1);
          Serial.printf("key pressed:%d",i);
          midimessage.note_pitch = normal_note_sequence[i];
          midimessage.note_state = 1;
        }
        else
        {
          // MIDI.sendNoteOn(normal_note_sequence[i], 0, 1);
          Serial.printf("key relesed:%d",i);
          midimessage.note_pitch = normal_note_sequence[i];
          midimessage.note_state = 0;
        }
      }
      // if (QueueHandle_usb != NULL && uxQueueSpacesAvailable(QueueHandle_usb) > 0)
      //{

      // int ret = xQueueSend(QueueHandle_usb, (void *)&midimessage, 0);

      if (midimessage.note_state != 0)
      {
        //Serial.print(midimessage.note_pitch);
      }
      /*
        if (ret == pdTRUE)
        {
        }
        else if (ret == errQUEUE_FULL)
        {
          Serial.println("The `TaskReadFromSerial` was unable to send data into the Queue");
        }
        */
      //}
    }
    delay(2);
  }
}
/*
void TaskWriteToMidi(void *pvParameters)
{
  midi_message_t midimessage;
  while (1)
  {
    bool ble_ok = BLEMidiServer.isConnected();
    if (QueueHandle_usb != NULL)
    {
      int ret = xQueueReceive(QueueHandle_usb, &midimessage, portMAX_DELAY);
      if (ret == pdPASS)
      {
        // The message was successfully received - send it back to Serial port and "Echo: "
        // Serial.printf("receive!");
        if (midimessage.type == 0)
        {
          if (midimessage.note_state == 1)
          {
            MIDI.sendNoteOn(midimessage.note_pitch, 127, 1);
            if (ble_ok)
            {
              BLEMidiServer.noteOn(0, midimessage.note_pitch, 127);
            }
          }
          else if (midimessage.note_state == 0)
          {
            MIDI.sendNoteOff(midimessage.note_pitch, 0, 1);
            if (ble_ok)
            {
              BLEMidiServer.noteOff(0, midimessage.note_pitch, 127);
            }
          }
        }
        else if (midimessage.type == 1)
        {
          MIDI.sendControlChange(42, midimessage.note_pitch, 12);
        }
      }
      else if (ret == pdFALSE)
      {
        Serial.println("The `TaskWriteToMIDIUSB` was unable to receive data from the Queue");
      }
    }
  }
}
*/

void TaskFastLED(void *pvParameters)
{
  while (1)
  {
    if (fadeDirection == 1)
    { // fade up
      EVERY_N_MILLISECONDS(3)
      {
        fill_solid(leds, NUM_LEDS, CHSV(hue, sat, val));
        val = val + 1;
        if (val == 255)
        {
          fadeDirection = !fadeDirection; // reverse direction
        }
      }
    }

    if (fadeDirection == 0)
    { // fade down
      EVERY_N_MILLISECONDS(9)
      {
        fill_solid(leds, NUM_LEDS, CHSV(hue, sat, val));
        val = val - 1;
        if (val == 0)
        {
          fadeDirection = !fadeDirection; // reverse direction
        }
      }
    }

    FastLED.show();

    EVERY_N_MILLISECONDS(90)
    { // cycle around the color wheel over time
      hue++;
    }
  }
}

#define UI_OVERTIME 1000

void TaskOLED(void *pvParameters)
{
  char *helloword_string = "Ripi hELLO";
  char *showstring = NULL;
  message_t show_message;

  unsigned long last_event_time = 0;
  bool ui_flag;

  while (1)
  {
    u8g2.clearBuffer();

    if (QueueHandle_oled != NULL)
    {
      int ret = xQueueReceive(QueueHandle_oled, &show_message, 100);
      if (ret == pdPASS)
      {
        // helloword_string = "Ripi is not good.";
        helloword_string = show_message.message;
        ui_flag = true;
        last_event_time = millis();
      }
      else
      {
      }
    }

    if (ui_flag)
    {
      unsigned long temp = millis();
      if (temp - last_event_time > UI_OVERTIME)
      {
        ui_flag = 0;
        helloword_string = "Ripi is really good.";
      }
    }

    u8g2.setCursor(0, 15);
    u8g2.print(helloword_string);
    u8g2.sendBuffer();
  }
}

void loop()
{
}
