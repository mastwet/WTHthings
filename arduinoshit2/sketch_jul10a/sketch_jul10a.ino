#include <Arduino.h>



#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif



U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(U8G2_R0, /* clock=*/ 21, /* data=*/ 40, /* reset=*/ U8X8_PIN_NONE);


void setup(void) {
  u8g2.begin();  
}

void loop(void) {
  u8g2.firstPage();
  do {
    u8g2.drawHLine(0,0,10);
    u8g2.drawHLine(0,31,10);
  
    u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.drawStr(0,24,"Hello World!");
  } while ( u8g2.nextPage() );
  //delay(1000);
}
