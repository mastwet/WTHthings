#include "SPI.h"
#include "TFT_eSPI.h" //gc9107 is basically the same as st7789 driver initialization
#include "pin_config.h"
#include <Arduino.h>


#include "Adafruit_Keypad.h"

// define your specific keypad here via PID
#define KEYPAD_PID3844
// define your pins here
// can ignore ones that don't apply
#define R1    33
#define R2    34
#define R3    35
#define R4    36
#define C1    37
#define C2    38
#define C3    39
#define C4    48
// leave this import after the above configuration
#include "keypad_config.h"

Adafruit_Keypad customKeypad = Adafruit_Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);
HardwareSerial MySerial(1);

TFT_eSPI tft = TFT_eSPI();
#define TDELAY 500
void setup() {
  MySerial.begin(9600, SERIAL_8N1, 17, 18);
  customKeypad.begin();

  pinMode(PIN_LCD_BL, OUTPUT);
  digitalWrite(PIN_LCD_BL, HIGH);

  tft.begin();
  tft.setRotation(2);

  digitalWrite(PIN_LCD_BL, LOW); // turn on backlight
  delay(5000);
  tft.fillScreen(TFT_BLACK);

  // Set "cursor" at top left corner of display (0,0) and select font 4
  tft.setCursor(0, 0, 4);

  // Set the font colour to be white with a black background
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.invertDisplay(true); // Where i is true or false
    // We can now plot text on screen using the "print" classss 

  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.println("XbotBed");

  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.println("Have a nice day!");

//  tft.setTextColor(TFT_BLUE, TFT_BLACK);
//  tft.println("Beethoven.mp3");

  delay(5000);
}

void loop() {
  String mcu_message="";
  // put your main code here, to run repeatedly:
  customKeypad.tick();

  while(customKeypad.available()){
    keypadEvent e = customKeypad.read();
    char a = (char)e.bit.KEY;
    MySerial.print(a);
    if(a=='1' && e.bit.EVENT == KEY_JUST_PRESSED){
        tft.setCursor(0, 0, 4);
         tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.println("playing beichuang");
    }
    if(a=='2' && e.bit.EVENT == KEY_JUST_PRESSED){
        tft.setCursor(0, 0, 4);
         tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.println("playing white noise");
    }
    if(a=='3' && e.bit.EVENT == KEY_JUST_PRESSED){
        tft.setCursor(0, 0, 4);
         tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.println("playing mom voice");
    }
    if(e.bit.EVENT == KEY_JUST_PRESSED) MySerial.println(" pressed");
    else if(e.bit.EVENT == KEY_JUST_RELEASED) MySerial.println(" released");
  }
  mcu_message = MySerial.readString();
  if(mcu_message.length() != 0){
    MySerial.println(mcu_message);
    tft.setCursor(0, 0, 4);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println(mcu_message);
  }

  delay(10);

}
