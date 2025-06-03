#include "keypad.h"
#include "Adafruit_Keypad.h"
void setup() {
  // put your setup code here, to run once:
  keypad_init();
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  keypadEvent e=keypad_scan();
  if(e != NULL){
    Serial.print((char)e.bit.KEY);
    if(e.bit.EVENT == KEY_JUST_PRESSED) Serial.println(" pressed");
    else if(e.bit.EVENT == KEY_JUST_RELEASED) Serial.println(" released");
  }

}
