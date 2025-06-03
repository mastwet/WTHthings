#include "HardwareSerial.h"

#include <U8g2lib.h>
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE, /* clock=*/4, /* data=*/2);

#define PACKET_START (0xFA)
#define PACKET_END (0xFF)

// HardwareSerial Serial1(1);

uint8_t packet[1024];
int packet_idx = 0;
int packet_en = 1;

// Declare a function type for menu actions.
typedef void (*menuAction)();
typedef String SubMenuItem;

struct MenuItem {
  String func_name;     // The name of the function to be displayed
  menuAction func_ptr;  // Function pointer for the function to be executed
};

void doSomething() {}
void doSomethingElse() {}

MenuItem menu_items[] = {
  { "Open OTA setting", &doSomething },
  { "Reload Config File", &doSomethingElse },
  { "Wireless OTA Update", &doSomething },
  { "KeyMode:", &doSomethingElse },
  { "LightMode:", &doSomethingElse },
};

SubMenuItem SubMenu_Mode_Items[] = {
  "Keyboard",
  "Midi",
  "Special"
};

SubMenuItem SubMenu_Light_Items[] = {
  "fluid",
  "shine",
  "nolight"
};




void setup() {

  u8g2.setBusClock(800000);
  u8g2.begin();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  // Start the main serial port for debugging
  Serial.begin(115200);

  // Start the second serial port
  Serial1.begin(115200, SERIAL_8N1, 6, 5);

  //Serial.println("Setup complete");
}

int current_index = 1;
int key_modeconfig = 0;
int light_modeconfig = 0;

void loop() {

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 32, menu_items[current_index].func_name.c_str());
  if(current_index == 3){
    u8g2.drawStr(0, 48,SubMenu_Mode_Items[key_modeconfig].c_str());
  }
  else if(current_index == 4){
    u8g2.drawStr(0, 48,SubMenu_Light_Items[light_modeconfig].c_str());
  }
  u8g2.sendBuffer();


  // Check if there is data to read
  while (Serial1.available()) {
    uint8_t data = Serial1.read();
    /*
    if (data == PACKET_START) {
      packet_idx = 0;
      packet_en = 1;
    }
    */
    if (packet_en == 1) {

      packet[packet_idx++] = data;

      if (data == PACKET_END) {
        // Now we have a complete packet, process it
        if (packet[1] == 0x01) {    // Button
          if (packet[2] == 0x01) {  // Button Pressed
            Serial.printf("key %d pressed\n", packet[3]);
          } else if (packet[2] == 0x02) {  // Button Released
            Serial.printf("key %d released\n", packet[3]);
          }
        } else if (packet[1] == 0x02) {  // Button
          if (packet[2] == 0x01) {       // Button Pressed
            Serial.printf("encoder %d left\n", packet[3]);
          } else if (packet[2] == 0x02) {  // Button Released
            Serial.printf("encoder %d right\n", packet[3]);
          }
        } else if (packet[1] == 0x03) {  // Button
          Serial.printf("Button %d released\n", packet[2]);
          if (packet[2] == 0x01) {
            if (current_index < 4)
              current_index++;
            else
              current_index = 0;
          }
          if (packet[2] == 0x02) {
            if (current_index > 0)
              current_index--;
            else
              current_index = 4;
          }
          if (packet[2] == 0x03) {
            if(current_index == 3){
            if (key_modeconfig < 2)
              key_modeconfig++;
            else
              key_modeconfig = 0;
            }
            if(current_index == 4){
            if (light_modeconfig < 2)
              light_modeconfig++;
            else
              light_modeconfig = 0;
            }
          }
        }
        packet_idx = 0;
        //packet_en = 0;
      }
    }
  }
}
