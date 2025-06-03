#include "HardwareSerial.h"

#define PACKET_START (0x0A)
#define PACKET_END   (0xFF)

HardwareSerial Serial1(1);

uint8_t packet[1024];
int packet_idx = 0;

void setup() {
  // Start the main serial port for debugging
  Serial.begin(115200);

  // Start the second serial port
  Serial1.begin(115200, SERIAL_8N1, 5, 6);

  Serial.println("Setup complete");
}

void loop() {
  // Check if there is data to read
  while (Serial1.available()) {
    uint8_t data = Serial1.read();

    if (data == PACKET_START) {
      packet_idx = 0;
    }

    packet[packet_idx++] = data;

    if (data == PACKET_END) {
      // Now we have a complete packet, process it
      if (packet[1] == 0x01) { // Button
        if (packet[2] == 0x01) { // Button Pressed
          Serial.printf("Button %d pressed\n", packet[3]);
        } else if (packet[2] == 0x02) { // Button Released
          Serial.printf("Button %d released\n", packet[3]);
        }
      }
      packet_idx = 0;
    }
  }
}
