#define MJPEG_FILENAME "/1.mjpeg"
#define MJPEG_BUFFER_SIZE (240 * 240 * 2 / 4)
#include <WiFi.h>
#include <FS.h>
#include <SD.h>
#include <SD_MMC.h>
#include "DHT.h"

#include <Arduino_GFX_Library.h>
#define TFT_BRIGHTNESS 128

#define DHTPIN 14
#define DHTTYPE DHT11   // DHT 11

#define MISO     2
#define SCK    14  //SCL
#define MOSI    15  //SDA
#define SD_CS   13
#define TFT_CS   5
#define TFT_BLK 22
#define TFT_DC  27
#define TFT_RST 33


Arduino_DataBus *bus = new Arduino_HWSPI(27 /* DC */, 5 /* CS */, SCK, MOSI, MISO);
Arduino_GC9A01  *gfx = new Arduino_GC9A01(bus, 33, 1, true);


#include "MjpegClass.h"
static MjpegClass mjpeg;
DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  WiFi.mode(WIFI_OFF);
  Serial.begin(115200);
  dht.begin();

  // Init Video
  gfx->begin();
  gfx->fillScreen(BLACK);

#ifdef TFT_BL
  ledcAttachPin(TFT_BL, 1);     // assign TFT_BL pin to channel 1
  ledcSetup(1, 12000, 8);       // 12 kHz PWM, 8-bit resolution
  ledcWrite(1, TFT_BRIGHTNESS); // brightness 0 - 255
#endif

  // Init SD card
  SPI.begin(SCK, MISO, MOSI, SD_CS);
  if( !SD.begin(SD_CS) )
  // if (!SD.begin(SC, SPI, 80000000)) /* SPI bus mode */
  // if ((!SD_MMC.begin()) && (!SD_MMC.begin())) /* 4-bit SD bus mode */
  // if ((!SD_MMC.begin("/sdcard", true)) && (!SD_MMC.begin("/sdcard", true))) /* 1-bit SD bus mode */
  {
    Serial.println(F("ERROR: SD card mount failed!"));
    gfx->println(F("ERROR: SD card mount failed!"));
  }
  else{
    while(1){
      File vFile = SD.open(MJPEG_FILENAME);
      // File vFile = SD_MMC.open(MJPEG_FILENAME);
      if (!vFile || vFile.isDirectory()){
        Serial.println(F("ERROR: Failed to open " MJPEG_FILENAME " file for reading"));
        gfx->println(F("ERROR: Failed to open " MJPEG_FILENAME " file for reading"));
      }
      else{
        uint8_t *mjpeg_buf = (uint8_t *)malloc(MJPEG_BUFFER_SIZE);
        if (!mjpeg_buf){
          Serial.println(F("mjpeg_buf malloc failed!"));
        }
        else{
          Serial.println(F("MJPEG video start"));
          mjpeg.setup(vFile, mjpeg_buf, gfx, true);
          Serial.println(mjpeg.readMjpegBuf());
          
          while (mjpeg.readMjpegBuf()){ // Read video
            mjpeg.drawJpg();            // Play video
          }
          
          Serial.println(mjpeg.readMjpegBuf());
          Serial.println(F("MJPEG video end"));
          vFile.close();
        }
      }
    }
  }

#ifdef TFT_BL
  delay(60000);
  ledcDetachPin(TFT_BL);
#endif
  gfx->displayOff();
  esp_deep_sleep_start();
}
  
void loop()
{
}
