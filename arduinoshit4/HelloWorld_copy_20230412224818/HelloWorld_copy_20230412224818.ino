/*

  HelloWorld.ino

  Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)

  Copyright (c) 2016, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.
    
  * Redistributions in binary form must reproduce the above copyright notice, this 
    list of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  

*/

#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif


U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 36, /* data=*/ 37, /* cs=*/ 21, /* dc=*/ 48, /* reset=*/ 47);


void setup(void) {
  u8g2.begin();
  u8g2.setFont(u8g2_font_micro_tr);	// choose a suitable font
}

void loop(void) {
  u8g2.clearBuffer();					// clear the internal memory
  u8g2.drawButtonUTF8(25, 6, U8G2_BTN_INV, u8g2.getDisplayWidth()-25*2,  25,  2, "Section 1:Kick" );
  u8g2.setCursor(3, 20);
  u8g2.print("1");
  u8g2.setCursor(3, 32);
  u8g2.print("2");
  u8g2.setCursor(3, 45);
  u8g2.print("3");
  u8g2.setCursor(3, 59);
  u8g2.print("4");
  for(int i=0;i<8;i++){
    u8g2.drawFrame(16+14*i,13,10,10);
  }
  for(int i=0;i<8;i++){
    u8g2.drawFrame(16+14*i,25,10,10);
  }
  for(int i=0;i<8;i++){
    u8g2.drawFrame(16+14*i,37,10,10);
  }
  for(int i=0;i<8;i++){
    u8g2.drawFrame(16+14*i,49,10,10);
  }
  u8g2.sendBuffer();					// transfer internal memory to the display
  delay(1000);  
}

