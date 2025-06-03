#define START_BYTE 0x0A
#define END_BYTE 0xFF
#define KEY_EVENT 0x01
#define PRESS 0x02
#define RELEASE 0x01

void setup() {
  Serial.begin(115200);   // Initialize serial communication with computer
   Serial1.begin(115200, SERIAL_8N1, 4/*rx*/, 5/*tx*/); // Rx = 4, Tx = 5 will work for ESP32, S2, S3 and C3
}

void loop() {
  if (Serial1.available()) {
    byte startByte = Serial1.read();
    
    if (startByte == START_BYTE) {
      while (!Serial1.available());  // Wait for next byte
      byte eventByte = Serial1.read();
      
      if (eventByte == KEY_EVENT) {
        while (!Serial1.available());  // Wait for next byte
        byte actionByte = Serial1.read();
        String action = (actionByte == PRESS) ? "Pressed" : (actionByte == RELEASE) ? "Released" : "Unknown action";
        
        while (!Serial1.available());  // Wait for next byte
        byte keyByte = Serial1.read();
        String key = "Key " + String(keyByte, HEX);
        
        while (!Serial1.available());  // Wait for next byte
        byte endByte = Serial1.read();
        
        if (endByte == END_BYTE) {
          Serial.println(key + " " + action);
        }
      }
    }
  }
}
