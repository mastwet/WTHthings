
#include <FastLED.h>
#define LED_PIN 11
#define NUM_LEDS 16
CRGB leds[NUM_LEDS];

uint8_t hue = 0;
uint8_t sat = 255;
uint8_t val = 0;
boolean fadeDirection = 1;  // [1=fade up, 0=fade down]

#include <U8g2lib.h>
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE, /* clock=*/18, /* data=*/17);  // ESP32 Thing, HW I2C with pin remapping

void setup() {
  // put your setup code here, to run once:

  u8g2.setBusClock(800000);
  u8g2.begin();
  u8g2.setFont(u8g2_font_ncenB08_tr);

  //FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
}

void loop() {
  u8g2.clearBuffer();
  u8g2.setCursor(0, 15);
  u8g2.print("hahahahhahaah");
  u8g2.sendBuffer();
  /*
  // put your main code here, to run repeatedly:
  if (fadeDirection == 1) {  //fade up
    EVERY_N_MILLISECONDS(3) {
      fill_solid(leds, NUM_LEDS, CHSV(hue, sat, val));
      val = val + 1;
      if (val == 255) {
        fadeDirection = !fadeDirection;  //reverse direction
      }
    }
  }

  if (fadeDirection == 0) {  //fade down
    EVERY_N_MILLISECONDS(9) {
      fill_solid(leds, NUM_LEDS, CHSV(hue, sat, val));
      val = val - 1;
      if (val == 0) {
        fadeDirection = !fadeDirection;  //reverse direction
      }
    }
  }

  FastLED.show();

  EVERY_N_MILLISECONDS(90) {  //cycle around the color wheel over time
    hue++;
  }
  */
}
