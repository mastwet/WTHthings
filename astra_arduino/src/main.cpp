#include <Arduino.h>

#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#include "astra_rocket.h"
#include "pinmap.h"
void device_init();

void setup() {
    //device_init();
    astraCoreInit();
}

void loop() {
    astraCoreLoop();
}

void device_init(){
    pinMode(KEY_PIN_1,INPUT_PULLUP);
    pinMode(KEY_PIN_2,INPUT_PULLUP);
    pinMode(KEY_PIN_3,INPUT_PULLUP);
    pinMode(KEY_PIN_4,INPUT_PULLUP);
}