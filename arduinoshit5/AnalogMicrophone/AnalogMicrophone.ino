/*
   Mono analog microphone example using electret mike on A0
   Run using the Arduino Serial Plotter to see waveform.
   Released to the Public Domain by Earle F. Philhower, III

   Wire the mike's VCC to 3.3V on the Pico, connect the mike's
   GND to a convenient Pico GND, and then connect mike OUT to A0
*/

#include <ADCInput.h>

ADCInput adc0(A0);
// For stereo/dual mikes, could use this line instead
//   ADCInput(A0, A1);

void setup() {
  Serial.begin(9600);
  pinMode(18,OUTPUT);
  pinMode(19,OUTPUT);
  pinMode(20,OUTPUT);
  pinMode(21,OUTPUT);
  adc0.begin(8000);
}

void loop() {

  // digitalWrite(18,HIGH);
  // digitalWrite(19,LOW);
  // digitalWrite(20,LOW);
  // digitalWrite(21,LOW);
  // Serial.printf("channel1:%d\n", adc0.read());

  // digitalWrite(18,HIGH);
  // digitalWrite(19,HIGH);
  // digitalWrite(20,LOW);
  // digitalWrite(21,LOW);
  // Serial.printf("channel2:%d\n", adc0.read());

  // digitalWrite(18,HIGH);
  // digitalWrite(19,HIGH);
  // digitalWrite(20,HIGH);
  // digitalWrite(21,LOW);
  // Serial.printf("channel3:%d\n", adc0.read());

  // digitalWrite(18,HIGH);
  // digitalWrite(19,HIGH);
  // digitalWrite(20,HIGH);
  // digitalWrite(21,HIGH);
  // Serial.printf("channel4:%d\n", adc0.read());

  // digitalWrite(18,HIGH);
  // digitalWrite(19,HIGH);
  // digitalWrite(20,LOW);
  // digitalWrite(21,HIGH);
  // Serial.printf("channel5:%d\n", adc0.read());

  // digitalWrite(18,HIGH);
  // digitalWrite(19,LOW);
  // digitalWrite(20,LOW);
  // digitalWrite(21,HIGH);
  // Serial.printf("channel6:%d\n", adc0.read());

  // digitalWrite(18,HIGH);
  // digitalWrite(19,LOW);
  // digitalWrite(20,HIGH);
  // digitalWrite(21,HIGH);
  Serial.printf("channel7:%d\n", adc0.read());

  // Serial.printf("channel8:%d\n", adc0.read());

  // Serial.printf("channel9:%d\n", adc0.read());

  // Serial.printf("channel10:%d\n", adc0.read());

  // Serial.printf("channel11:%d\n", adc0.read());

  // Serial.printf("channel12:%d\n", adc0.read());

  // Serial.printf("channel13:%d\n", adc0.read());

  // Serial.printf("channel14:%d\n", adc0.read());

  // Serial.printf("channel15:%d\n", adc0.read());

  // Serial.printf("channel16:%d\n", adc0.read());
  delay(1000);
}
