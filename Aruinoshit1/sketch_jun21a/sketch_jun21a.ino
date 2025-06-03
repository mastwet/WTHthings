byte variable[80];
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, 6, 5);
}

#define PACKET_START 0XFA

byte packet[30] = { 0 };
int packet_idx = 0;
int encoder_state[4] = {0};

void loop() {
  // put your main code here, to run repeatedly:
  while (Serial1.available()) {
    byte data = Serial1.read();
    packet[packet_idx++] = data;

    if (data == 0xff) {

      /////////////////////////////////////////

      if (packet[1] == 0x01) {    // Button
        //midimessage.type = 0;
        //midimessage.note_pitch = key_note_map[packet[3] - 1];
        if (packet[2] == 0x01) {  // Button Pressed
            Serial.printf("key %d pressed\n", packet[3] - 1);
            //midimessage.note_state = 1;
        } else if (packet[2] == 0x02) {  // Button Released
            Serial.printf("key %d released\n", packet[3] - 1);
            //midimessage.note_state = 0;
        }
      }

      ////////////////////////////////////////

      if (packet[1] == 0xb2) {
        //if (current_mode == 0) {
        if (packet[2] == 0xca) {
          //midimessage.type = 1;
          if (encoder_state[packet[3] - 1] > 0) {
            encoder_state[packet[3] - 1] -= 5;
          }
          Serial.printf("encoder:%d\n", encoder_state[packet[3] - 1]);
          //midimessage.note_pitch = encoder_state[packet[3] - 1];
        } 
        else if (packet[2] == 0xcb) {
          //midimessage.type = 1;
          if (encoder_state[packet[3] - 1] < 127) {
            encoder_state[packet[3] - 1] += 5;
          }
          Serial.printf("encoder:%d\n", encoder_state[packet[3] - 1]);
          //midimessage.note_pitch = encoder_state[packet[3] - 1];
        }


        //message.type = midimessage.type;
        //message.name = 0;
        //message.value = midimessage.note_pitch;

      }
    }
  }
  packet_idx = 0;
  //delay(2);
}
