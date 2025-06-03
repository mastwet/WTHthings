
#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ 4, /* data=*/ 2);   // ESP32 Thing, HW I2C with pin remapping

void setup(void) {
  u8g2.begin();
}

void loop(void) {
  u8g2.clearBuffer();					// clear the internal memory
  
  u8g2.setFont(u8g2_font_ncenB14_tr);
  u8g2.drawStr(30,25,"KNOB1");
  u8g2.setFont(u8g2_font_ncenB08_tr);	// choose a suitable font
  u8g2.drawStr(10,40,"Channel:0 NOTE:A0");
  u8g2.drawStr(40,50,"Value:10");

  u8g2.drawFrame(14,52,100,10);
  u8g2.drawBox(14,52,50,10);
  u8g2.sendBuffer();					// transfer internal memory to the display
  delay(1000);  
}

