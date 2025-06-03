#include <Arduino_GFX_Library.h>

#define MISO     2
#define SCK    14  //SCL
#define MOSI    15  //SDA
#define SD_CS   13
#define TFT_CS   5
#define TFT_BLK 22
#define TFT_DC  27
#define TFT_RST 33

/* More dev device declaration: https://github.com/moononournation/Arduino_GFX/wiki/Dev-Device-Declaration */
#if defined(DISPLAY_DEV_KIT)
Arduino_GFX *gfx = create_default_Arduino_GFX();
#else /* !defined(DISPLAY_DEV_KIT) */

Arduino_DataBus *bus = new Arduino_HWSPI(27 /* DC */, 5 /* CS */, SCK, MOSI, MISO);
Arduino_GC9A01  *gfx = new Arduino_GC9A01(bus, 33, 1, true);

String gif[5] = {"/1.gif","/2.gif","/3.gif","/4.gif","/5.gif"};

#endif /* !defined(DISPLAY_DEV_KIT) */
/*******************************************************************************
 * End of Arduino_GFX setting
 ******************************************************************************/


#include <SD.h>

#include "GifClass.h"
static GifClass gifClass;
int pinCS = 53;
void setup()
{
  Serial.begin(115200);
  pinMode(pinCS, OUTPUT);
  // Init Display
  gfx->begin();
  gfx->fillScreen(BLACK);

#ifdef DF_GFX_BL
  pinMode(DF_GFX_BL, OUTPUT);
  digitalWrite(DF_GFX_BL, HIGH);
#endif

  SPI.begin(SCK, MISO, MOSI, SD_CS);
  if (!SD.begin(SD_CS))
  {
    Serial.println(F("ERROR: File System Mount Failed!"));
    gfx->println(F("ERROR: File System Mount Failed!"));
    exit(0);
  }

  
  File gifFile = SD.open(gif[0], "r");
  if (!gifFile || gifFile.isDirectory())
  {
    Serial.println(F("ERROR: open gifFile Failed!"));
    gfx->println(F("ERROR: open gifFile Failed!"));
  }
  else
  {
    // read GIF file header
    gd_GIF *gif = gifClass.gd_open_gif(&gifFile);
    if (!gif)
    {
      Serial.println(F("gd_open_gif() failed!"));
    }
    else
    {
      uint8_t *buf = (uint8_t *)malloc(gif->width * gif->height);
      if (!buf)
      {
        Serial.println(F("buf malloc failed!"));
      }
      else
      {
        int16_t x = (gfx->width() - gif->width) / 2;
        int16_t y = (gfx->height() - gif->height) / 2;

        Serial.println(F("GIF video start"));
        uint32_t t_fstart, t_delay = 0, t_real_delay, delay_until;
        int32_t res = 1;
        uint32_t duration = 0, remain = 0;
        while (res > 0)
        {
          t_fstart = millis();
          t_delay = gif->gce.delay * 10;
          res = gifClass.gd_get_frame(gif, buf);
          if (res < 0)
          {
            Serial.println(F("ERROR: gd_get_frame() failed!"));
            break;
          }
          else if (res > 0)
          {
            gfx->drawIndexedBitmap(x, y, buf, gif->palette->colors, gif->width, gif->height);
//
//            t_real_delay = t_delay - (millis() - t_fstart);
//            duration += t_delay;
//            remain += t_real_delay;
//            delay_until = millis() + t_real_delay;
//            do
//            {
//              delay(1);
//            } while (millis() < delay_until);
          }
        }
        Serial.println(F("GIF video end"));
        Serial.print(F("duration: "));
        Serial.print(duration);
        Serial.print(F(", remain: "));
        Serial.print(remain);
        Serial.print(F(" ("));
        Serial.print(100.0 * remain / duration);
        Serial.println(F("%)"));

        gifClass.gd_close_gif(gif);
        free(buf);
        gifFile.close();
      }
    }
  }
}

String gif_read = gif[1];

void loop()
{
/* Wio Terminal */
  while (Serial.available()) {
    char inByte = (char)Serial.read();
    if(inByte == '1'){
        Serial.println("happy!");
        gif_read = gif[2];
    }
  }

  File gifFile = SD.open(gif_read, "r");
  gif_read = gif[1];
  if (!gifFile || gifFile.isDirectory())
  {
    Serial.println(F("ERROR: open gifFile Failed!"));
    gfx->println(F("ERROR: open gifFile Failed!"));
  }
  else
  {
    // read GIF file header
    gd_GIF *gif = gifClass.gd_open_gif(&gifFile);
    if (!gif)
    {
      Serial.println(F("gd_open_gif() failed!"));
    }
    else
    {
      uint8_t *buf = (uint8_t *)malloc(gif->width * gif->height);
      if (!buf)
      {
        Serial.println(F("buf malloc failed!"));
      }
      else
      {
        int16_t x = (gfx->width() - gif->width) / 2;
        int16_t y = (gfx->height() - gif->height) / 2;

        Serial.println(F("GIF video start"));
        uint32_t t_fstart, t_delay = 0, t_real_delay, delay_until;
        int32_t res = 1;
        uint32_t duration = 0, remain = 0;
        while (res > 0)
        {
          t_fstart = millis();
          t_delay = gif->gce.delay * 10;
          res = gifClass.gd_get_frame(gif, buf);
          if (res < 0)
          {
            Serial.println(F("ERROR: gd_get_frame() failed!"));
            break;
          }
          else if (res > 0)
          {
            gfx->drawIndexedBitmap(x, y, buf, gif->palette->colors, gif->width, gif->height);
//
//            t_real_delay = t_delay - (millis() - t_fstart);
//            duration += t_delay;
//            remain += t_real_delay;
//            delay_until = millis() + t_real_delay;
//            do
//            {
//              delay(1);
//            } while (millis() < delay_until);
          }
        }
        Serial.println(F("GIF video end"));
        Serial.print(F("duration: "));
        Serial.print(duration);
        Serial.print(F(", remain: "));
        Serial.print(remain);
        Serial.print(F(" ("));
        Serial.print(100.0 * remain / duration);
        Serial.println(F("%)"));

        gifClass.gd_close_gif(gif);
        free(buf);
        gifFile.close();
    }
  }
}
}
