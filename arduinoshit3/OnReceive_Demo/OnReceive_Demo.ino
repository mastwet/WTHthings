

#include <Arduino.h>

#define USE_INTERNAL_PIN_LOOPBACK 1   // 1 uses the internal loopback, 0 for wiring pins 4 and 5 externally

#define DATA_SIZE 26    
#define BAUD 115200    
#define TEST_UART 1   
#define RXPIN 6    
#define TXPIN 5      

volatile size_t sent_bytes = 0, received_bytes = 0;



void setup() {
  // UART0 will be used to log information into Serial Monitor
  Serial.begin(115200);

  // UART1 will have its RX<->TX cross connected
  // GPIO4 <--> GPIO5 using external wire
  Serial1.begin(BAUD, SERIAL_8N1, RXPIN, TXPIN); // Rx = 4, Tx = 5 will work for ESP32, S2, S3 and C3

}

void loop() {
  //Serial.println("114514");
  //delay(1000);
  if(Serial1.available()){
    Serial.print((char)Serial1.read());
  //Serial.println();
  }
}
