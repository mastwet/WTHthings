#ifndef __KEYPAD__
#define __KEYPAD__

#include <Arduino.h>
#include "Adafruit_Keypad.h"

static const byte ROWS = 3; // rows
static const byte COLS = 4; // columns

static char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
};

static byte rowPins[ROWS] = {39, 38, 48}; //connect to the row pinouts of the keypad
static byte colPins[COLS] = {47, 18, 45, 46}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Adafruit_Keypad customKeypad = Adafruit_Keypad( makeKeymap(keys), colPins, rowPins, COLS, ROWS);

void keypad_init(){
    customKeypad.begin();
}


keypadEvent keypad_scan(){
    customKeypad.tick();
    if(customKeypad.available()){
        return customKeypad.read();
    }
    else{
        return NULL;
    }
}

#endif