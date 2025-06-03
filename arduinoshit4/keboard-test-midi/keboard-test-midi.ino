#include <ShiftIn.h>
// Init ShiftIn instance with a single chip
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

bool pinState_key[25];
bool encoder_data[8];
unsigned int encoder_state[4] = { 0 };

int remap[25] = {16,17,18,19,20,21,22,23,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,24};

typedef struct
{
  bool val;
  bool last_val;
} KEY;

KEY key[10] = { false };

int yjoy_pre;
int xjoy_pre;

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
  Serial.println(encoder_state[encoder_num]);
}



bool pinState[8];

void key_print(bool data, int i) {
  if (data != pinState_key[i]) {
    pinState_key[i] = data;
    Serial.print("Pin ");
    Serial.print(remap[i]);
    Serial.print(": ");
    Serial.println(data);
  }
}

void button_print(int i) {
  if (key[i].last_val == 1 && key[i].val == 0) {
    // send midi message to queue
    Serial.println(i);
    delay(200);
  }
}

void setup() {
  Serial.begin(115200);
  // declare pins: pLoadPin, clockEnablePin, dataPin, clockPin
  shift_key.begin(KEYPAD_PIN_LOAD, 9, KEYPAD_PIN_DATA, KEYPAD_PIN_CLK);
  // declare pins: pLoadPin, clockEnablePin, dataPin, clockPin
  shift_button.begin(BUTTON_PIN_LOAD, 9, BUTTON_PIN_DATA, BUTTON_PIN_CLK);
  // declare pins: pLoadPin, clockEnablePin, dataPin, clockPin
  shift_encoder.begin(ENCODER_PIN_LOAD, 9, ENCODER_PIN_DATA, ENCODER_PIN_CLK);

  pinMode(3, INPUT);
  pinMode(48, INPUT);
  pinMode(45, INPUT);
  
}

void loop() {
  //read keypad
  shift_key.update();
  for (uint8_t i = 0, n = shift_key.getDataWidth(); i < n; i++) {
    bool data = shift_key.state(i);
    key_print(data, i);
  }
  bool data = digitalRead(3);
  key_print(data, 24);
  //read encode
  // read encoder value
  shift_encoder.update();
  for (int i = 0; i < 8; i++) {
    encoder_data[i] = shift_encoder.state(i);
    if (encoder_data[i] != pinState[i]) {
      if (pinState[0] == 1 && encoder_data[0] == 0) {
        // falling edge
        if (encoder_data[1] == HIGH) {
          encoder_midi_queue_send(0, 0);
        }
        if (encoder_data[1] == LOW) {
          encoder_midi_queue_send(0, 1);
        }
      }
      if (pinState[2] == 1 && encoder_data[2] == 0) {
        if (encoder_data[3] == HIGH)   {
          encoder_midi_queue_send(1, 1);
        }
        if (encoder_data[3] == LOW) {
          encoder_midi_queue_send(1, 0);
        }
      }
      if (pinState[4] == 1 && encoder_data[4] == 0) {
        // falling edge
        if (encoder_data[5] == HIGH) {
          encoder_midi_queue_send(2, 1);
        }
        if (encoder_data[5] == LOW) {
          encoder_midi_queue_send(2, 0);
        }
      }
      if (pinState[6] == 1 && encoder_data[6] == 0) {
        // falling edge
        if (encoder_data[7] == HIGH) {
          encoder_midi_queue_send(3, 1);
        }
        if (encoder_data[7] == LOW) {
          encoder_midi_queue_send(3, 0);
        }
      }
    }
    pinState[i] = encoder_data[i];
  }
  //button
  shift_button.update();
  for (int i = 0; i < 8; i++) {
    key[i].val = shift_button.state(i);
    // if time is greater than debounce delay
    button_print(i);
    key[i].last_val = key[i].val;
  }

  key[8].val = digitalRead(48);
    // if time is greater than debounce delay
  button_print(8);
  key[8].last_val = key[8].val;
  
  key[9].val = digitalRead(45);
  button_print(9);
  key[9].last_val = key[9].val;
  /*
  int x_joy = analogRead(9);
  Serial.printf("xjoy:%d \r\n",x_joy);
  xjoy_pre = x_joy;

  int y_joy = analogRead(10);
  Serial.printf("yjoy:%d \r\n",y_joy);
  yjoy_pre = y_joy;
  */
  
}