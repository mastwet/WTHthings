#include <Arduino.h>
#include "Button.h"

#define DATA_PIN 35
#define CLOCK_PIN 37
#define LATCH_PIN 36

Button button(DATA_PIN, CLOCK_PIN, LATCH_PIN);

void setup() {
  Serial.begin(115200);
}

void loop() {
  for (byte i = 0; i < BUTTON_COUNT; i++) {
    if (button.isPressed(i)) {
      Serial.print("Button ");
      Serial.print(i);
      Serial.println(" is pressed.");
    }
  }
  delay(100);
}
