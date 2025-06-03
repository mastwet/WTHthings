#include "Adafruit_Keypad.h"
#include "keypad.h"

Adafruit_Keypad customKeypad = Adafruit_Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  // put your setup code here, to run once:
    customKeypad.begin();
    xTaskCreate(
            TaskKeypad
            ,  "4x4 Keypad Task" // A name just for humans
            ,  128        // The stack size can be checked by calling `uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);`
            ,  nullptr // Task parameter which can modify the task behavior. This must be passed as pointer to void.
            ,  1  // Priority
            ,  nullptr // Task handle is not used here - simply pass NULL
    );
}

void TaskKeypad( void *pvParameters ){
    for(;;){
      customKeypad.tick();
      while(customKeypad.available()){
        keypadEvent e = customKeypad.read();
        Serial.print((char)e.bit.KEY);
        if(e.bit.EVENT == KEY_JUST_PRESSED) Serial.println(" pressed");
        else if(e.bit.EVENT == KEY_JUST_RELEASED) Serial.println(" released");
      }
      delay(10);
    }
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
}
