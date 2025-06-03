#include <SPI.h>
// an arduino code to use the SPI communication on ESP32 to drive an 74hc165 shift register
// the code is based on the example code from the ESP32 SPI library
//
// the 74hc165 is connected to the ESP32 as follows:

#define HSPI_MOSI 37
#define HSPI_MISO 26
#define HSPI_SCLK 36
#define HSPI_SS 35

static const int spiClk = 1000000;  // 1 MHz
SPIClass *hspi = NULL;

#define chips 1  // number of 74hc165 chips

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  hspi = new SPIClass(HSPI);
  hspi->begin(HSPI_SCLK, HSPI_MISO, HSPI_MOSI, HSPI_SS);
  pinMode(hspi->pinSS(), OUTPUT);  // HSPI SS
  digitalWrite(hspi->pinSS(), HIGH);
  // hspi->setFrequency(spiClk);
  // hspi->setBitOrder(MSBFIRST);
  // hspi->setDataMode(SPI_MODE0);
}

void loop() {
  byte Switch[chips];  // to store the data from the 74hc165
  // to give 74hc165 a clock pulse,then use spi to read the data
  digitalWrite(hspi->pinSS(), HIGH);
  delay(1);
  digitalWrite(hspi->pinSS(), LOW);
  // to transfer data use spi
  hspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  for (byte i = 0; i < chips; i++) {
    Switch[i] = hspi->transfer(0);
    // then the data is stored in the Switch array
  }
  hspi->endTransaction();
  Serial.println(Switch[0]);
}
