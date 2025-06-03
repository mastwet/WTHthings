/* Basic Multi Threading Arduino Example
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
// Please read file README.md in the folder containing this example.

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

#define ANALOG_INPUT_PIN A0

#ifndef LED_BUILTIN
#define LED_BUILTIN 13  // Specify the on which is your LED
#endif

#define ROXMUX_74HC165_DELAY 1

// Define two tasks for Blink & AnalogRead.
void TaskBlink(void *pvParameters);
void TaskAnalogRead(void *pvParameters);
TaskHandle_t analog_read_task_handle;  // You can (don't have to) use this to be able to manipulate a task from somewhere else.

// The setup function runs once when you press reset or power on the board.
void setup() {
  // Initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  // Set up two tasks to run independently.
  uint32_t blink_delay = 1000;  // Delay between changing state on LED pin
  xTaskCreate(
    TaskBlink, "Task Blink"  // A name just for humans
    ,
    2048  // The stack size can be checked by calling `uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);`
    ,
    (void *)&blink_delay  // Task parameter which can modify the task behavior. This must be passed as pointer to void.
    ,
    2  // Priority
    ,
    NULL  // Task handle is not used here - simply pass NULL
  );
  Serial.printf("Basic Multi Threading Arduino Example\n");
  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}

void loop() {
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TaskBlink(void *pvParameters) {  // This is a task.
  uint32_t blink_delay = *((uint32_t *)pvParameters);
  pinMode(36, OUTPUT);  //clkpin
  pinMode(35, OUTPUT);  //loadPin
  pinMode(37, INPUT);   //datapin
  digitalWrite(36, LOW);
  digitalWrite(35, HIGH);
  unsigned char states;
  unsigned char data[8];

  for (;;) {  // A Task shall never return or exit.
    digitalWrite(35, LOW);
    delayMicroseconds(ROXMUX_74HC165_DELAY);
    digitalWrite(35, HIGH);
    for (int i = 7; i >= 0; i--) {
      uint8_t bit = digitalRead(37);
      bitWrite(states, i, bit);
      digitalWrite(36, HIGH);
      delayMicroseconds(ROXMUX_74HC165_DELAY);
      digitalWrite(36, LOW);
    }
    for (int i = 0; i < 8; i++) {
      data[i] = bitRead(states, (i & 0x07));
    }
  }
}
