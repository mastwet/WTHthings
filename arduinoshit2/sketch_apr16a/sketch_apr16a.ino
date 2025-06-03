#include <ShiftIn.h>

ShiftIn<2> shift_encoder_key;

#define PIN_DATA 35  // pin 9 on 74HC165 (DATA)
#define PIN_LOAD 36  // pin 1 on 74HC165 (LOAD)
#define PIN_CLK 36   // pin 2 on 74HC165 (CLK))

bool pinState[16];
bool encoder_data[16];

int a=0;
int b=1;

void setup() {
  Serial.begin(115200);
  shift_encoder_key.begin(PIN_LOAD, 8, PIN_DATA, PIN_CLK);
}

int num = 0;
void loop() {
  // put your main code here, to run repeatedly:
  shift_encoder_key.update();

  a = shift_encoder_key.state(0);
  Serial.println(a);
  if (a != b) { 
    // check channel B to see which way encoder is turning
    shift_encoder_key.update();
    if (shift_encoder_key.state(1) == HIGH) {  
      num = num+1;
    } 
    else{
      num = num-1;
    }
    Serial.println(num);
  }
  a = b;
  delay(2);
}
