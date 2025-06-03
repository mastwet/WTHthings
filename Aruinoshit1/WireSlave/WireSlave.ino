#include "Wire.h"

#define I2C_DEV_ADDR 0x14
#define SDA 1
#define SCL 2

uint32_t i = 0;

void onRequest(){
  Wire.print(i++);
  Wire.print(" Packets.");
  Serial.println("onRequest");
}

void onReceive(int len){
  Serial.printf("onReceive[%d]: ", len);
  while(Wire.available()){
    Serial.write(Wire.read());
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Wire.onReceive(onReceive);
  Wire.onRequest(onRequest);
  //Wire.begin((uint8_t)I2C_DEV_ADDR);
  Wire.begin((uint8_t)I2C_DEV_ADDR,SDA,SCL,100000);
  Serial.println("begin...");

/*
#if CONFIG_IDF_TARGET_ESP32
  char message[64];
  snprintf(message, 64, "%u Packets.", i++);
  Wire.slaveWrite((uint8_t *)message, strlen(message));
#endif
*/

}

void loop() {

}
