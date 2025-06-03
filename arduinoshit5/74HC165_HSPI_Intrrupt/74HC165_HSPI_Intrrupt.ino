/* 74HC165 ESP8266 ~ NodeMCU 1.0 (ESP-12E Module)
 * speed-test reading 18 shift (in) registers
 * 
 * adapted from article by João Alves
 * https://jpralves.net/post/2015/09/08/multiple-inputs-parallel-to-series.html
 * 
 * Pinos usados: 
     D0  (gpio16) - Load Pin (1) do 74HC165 
     D6  (hmiso)  - Q7 Pin (9) do 74HC165 
     D5  (hsclk)  - CLOCK Pin (2) do 74HC165 
 * Sketch que usa a comunicação SPI. 
*/

#include <SPI.h>

#define SPI_MISO 16
#define SPI_MOSI 5
#define SPI_SCLK 18
#define SPI_SS 6

const byte LATCH = 17;
const byte chips = 1;  // number of 74HC165 (8bit) chips used

static unsigned long lastMillis;
static unsigned long frameCount;
static unsigned int framesPerSecond;
const int seconds = 1;

void setup() {
  SPI.begin(SPI_SCLK, SPI_MISO, SPI_MOSI, SPI_SS);
  SPI.beginTransaction(SPISettings(115200, MSBFIRST, SPI_MODE3));  // 32MHz
  Serial.begin(115200);
  pinMode(LATCH, OUTPUT);
  digitalWrite(LATCH, HIGH);
}

void loop() {
  unsigned long now = millis();
  frameCount++;  // Once around the moon ...

  byte Switch[chips];

  digitalWrite(LATCH, HIGH);
  digitalWrite(LATCH, LOW);

  for (byte i = 0; i < chips; i++) {
    Switch[i] = SPI.transfer(0);
  }

  if (now - lastMillis >= seconds * 1000) {
    framesPerSecond = frameCount / seconds;

    for (byte i = 0; i < 8; i++) {
      Serial.print((Switch[0] & (1 << i)) != 0 ? 0 : 1);
    }

    Serial.print(" ");

    for (byte i = 0; i < 8; i++) {
      Serial.print((Switch[1] & (1 << i)) != 0 ? 0 : 1);
    }

    Serial.print(" ");
    Serial.print(framesPerSecond);
    Serial.println(" fps");

    frameCount = 0;
    lastMillis = now;
  }
}
