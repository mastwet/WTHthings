#include <Adafruit_TinyUSB.h>
#include <EEPROM.h>
#include <MIDI.h>
#include "USB.h"

const int buttonPin = 2;
const int debounceDelay = 50;
const int flashAddress = 0;

int buttonState;
int lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;

EEPROMClass  OTA_SEETINGS("eeprom0");

Adafruit_USBD_MIDI usb_midi;

#if ARDUINO_USB_CDC_ON_BOOT
#define HWSerial Serial0
#define USBSerial Serial
#else
#define HWSerial Serial

#endif

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  if (!OTA_SEETINGS.begin(0x500)) {
    delay(10000);
    ESP.restart();
  }
  toggleNumber();
}

void loop() {

}

void toggleNumber() {
  int number;
  OTA_SEETINGS.get(0, number);
  if(number){
    OTA_SEETINGS.put(0, 0);
    USBCDC USBSerial;
    USBSerial.begin(115200);
    while(1){
      USBSerial.println("hahahahahahahahaha");
    }
  }
  else{
    OTA_SEETINGS.put(0,1);
    USBCDC USBSerial;
    USBSerial.begin(115200);
    while(1){
      USBSerial.println("henghengaaaaaaaaaaaaa");
    }
  }
  delay(10000);
  ESP.restart();
}