#include <ShiftIn.h>

ShiftIn<1> shift_encoder;
ShiftIn<1> shift_key;

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

// Stop button is attached to PIN 0 (IO0)
#define BTN_STOP_ALARM 0

void readKey();
void taskEncoder(void *pvParameters);

void setup() {
  Serial.begin(115200);
  // declare pins: pLoadPin, clockEnablePin, dataPin, clockPin
  shift_encoder.begin(PIN_LOAD, 8, PIN_DATA, PIN_CLK);
  shift_key.begin(PIN2_LOAD, 8, PIN2_DATA, PIN2_CLK);
  xTaskCreate(
    taskEncoder, "Encoder Task"  // A name just for humans
    ,
    2048  // The stack size can be checked by calling `uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);`
    ,
    (void *)NULL,
    2  // Priority
    ,
    NULL  // Task handle is not used here - simply pass NULL
  );
}

int pre_state = 0;
bool data[8];
unsigned int encoder_state[4] = { 0 };
void readEncoder() {
  for (int i = 0; i < 8; i++) {
    data[i] = shift_encoder.state(i);
    if (data[i] != pinState[i]) {
      if (pinState[0] == 1 && data[0] == 0) {
        //falling edge
        if (data[1] == HIGH) {
          if (encoder_state[0] < 127) {
            encoder_state[0] += 1;
          }
          Serial.println(encoder_state[0]);
        }
        if (data[1] == LOW) {
          if (encoder_state[0] > 1) {
            encoder_state[0] -= 1;
          }
          Serial.println(encoder_state[0]);
        }
      }
      if (pinState[2] == 1 && data[2] == 0) {
        //falling edge
        if (data[3] == HIGH) {
          if (encoder_state[1] < 127) {
            encoder_state[1] += 1;
          }
          Serial.println(encoder_state[1]);
        }
        if (data[3] == LOW) {
          if (encoder_state[1] > 1) {
            encoder_state[1] -= 1;
          }
          Serial.println(encoder_state[1]);
        }
      }
      if (pinState[4] == 1 && data[4] == 0) {
        //falling edge
        if (data[5] == HIGH) {
          if (encoder_state[2] < 127) {
            encoder_state[2] += 1;
          }
          Serial.println(encoder_state[2]);
        }
        if (data[5] == LOW) {
          if (encoder_state[2] > 1) {
            encoder_state[2] -= 1;
          }
          Serial.println(encoder_state[2]);
        }
      }
      if (pinState[6] == 1 && data[6] == 0) {
        //falling edge
        if (data[7] == HIGH) {
          if (encoder_state[3] < 127) {
            encoder_state[3] += 1;
          }
          Serial.println(encoder_state[3]);
        }
        if (data[7] == LOW) {
          if (encoder_state[3] > 1) {
            encoder_state[3] -= 1;
          }
          Serial.println(encoder_state[3]);
        }
      }
    }
    pinState[i] = data[i];
  }
}
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 200;

void readKey() {
  for (int i = 0; i < 8; i++) {
    shift_key.update();
    data2[i] = shift_key.state(i);
    if (key_pinState[i] == 1 && data2[i] == 0 /* && ((millis()-lastDebounceTime > debounceDelay))*/) {
      Serial.print(i);
      Serial.println("pressed");
      lastDebounceTime = millis();
    }

    key_pinState[i] = data2[i];
  }
}

void taskEncoder(void *pvParameters) {
  while (1) {
    readEncoder();
    delay(1);
  }
}

void loop() {
  // Do whatever
  shift_encoder.update();
  //bool data = mux.read(0);
  //bool data2 = mux.read(1);
  //Serial.println(data);
  //Serial.println(data2);
  //readEncoder();
  readKey();
  //Serial.println("test...");
}
