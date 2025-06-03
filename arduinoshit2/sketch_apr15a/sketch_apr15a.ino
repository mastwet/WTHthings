#include <ShiftIn.h>

ShiftIn<2> shift_encoder_key;
ShiftIn<3> shift_key;

#define PIN_DATA 35  // pin 9 on 74HC165 (DATA)
#define PIN_LOAD 36  // pin 1 on 74HC165 (LOAD)
#define PIN_CLK 37   // pin 2 on 74HC165 (CLK))

#define PIN2_DATA 14  // pin 9 on 74HC165 (DATA)
#define PIN2_LOAD 13  // pin 1 on 74HC165 (LOAD)
#define PIN2_CLK 12   // pin 2 on 74HC165 (CLK))

bool pinState[24];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  shift_encoder_key.begin(PIN_LOAD, 8, PIN_DATA, PIN_CLK);
  shift_key.begin(PIN2_LOAD, 8, PIN2_DATA, PIN2_CLK);
}

typedef struct
{
  bool val;
  bool last_val;
} KEY;

KEY key[16] = {false};

int key_value_raw[24] = { 0 };
int key_value[24] = { 0 };

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 200;

bool encoder_data[8];

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.println("hello0");
  shift_key.update();
  //shift_encoder_key.update();
  for (int i = 0; i < shift_key.getDataWidth(); i++)
    //Serial.print(shift_key.state(i));  // get state of button i
    key_value_raw[i] = shift_key.state(i);
  for (int i = 0; i < 8; i++) {
    key_value[i] = key_value_raw[i + 16];
    key_value[i + 8] = key_value_raw[i + 8];
    key_value[i + 16] = key_value_raw[i];
  }


  Serial.print("keypad:");
  for (int i = 0; i < 24; i++)
    Serial.print(key_value[i]);  // get state of button i
    //Serial.print(shift_key.state(i));  // get state of button i
  Serial.println();
/*
  Serial.print("encoderkey:");

  for (int i = 0; i < 8; i++)
  {
    encoder_data[i] = shift_encoder_key.state(i);
    if (encoder_data[i] != pinState[i])
    {
      if (pinState[0] == 1 && encoder_data[0] == 0)
      {
        // falling edge
        if (encoder_data[1] == HIGH)
        {
          Serial.print("1");
          Serial.println("left");
        }
        if (encoder_data[1] == LOW)
        {
          Serial.print("1");
          Serial.println("right");
        }
      }
      if (pinState[2] == 1 && encoder_data[2] == 0)
      {
        if (encoder_data[3] == HIGH)
        {
          Serial.print("2");
          Serial.println("left");
        }
        if (encoder_data[3] == LOW)
        {
          Serial.print("2");
          Serial.println("right");
        }
      }
      if (pinState[4] == 1 && encoder_data[4] == 0)
      {
        // falling edge
        if (encoder_data[5] == HIGH)
        {
          Serial.print("3");
          Serial.println("left");
        }
        if (encoder_data[5] == LOW)
        {
          Serial.print("3");
          Serial.println("right");
        }
      }
      if (pinState[6] == 1 && encoder_data[6] == 0)
      {
        // falling edge
        if (encoder_data[7] == HIGH)
        {
          Serial.print("4");
          Serial.println("left");
        }
        if (encoder_data[7] == LOW)
        {
          Serial.print("4");
          Serial.println("right");
        }
      }
    }
  }
   
  for (int i = 8; i < 16; i++) {
    key[i-8].val = shift_encoder_key.state(i);
    // if time is greater than debounce delay
    if (key[i-8].last_val == 1 && key[i-8].val == 0) {
      // send midi message to queue
      Serial.print("key:");
      Serial.print(i-8);
    }
    lastDebounceTime = millis();
    key[i-8].last_val = key[i-8].val;
  }
  */


  for (int i = 0; i < shift_encoder_key.getDataWidth(); i++)
    Serial.print(shift_encoder_key.state(i));  // get state of button i

  Serial.println();

  delay(100);
  // int ret = xQueueSend(QueueHandle_usb, (void *)&midimessage, 0);
}

