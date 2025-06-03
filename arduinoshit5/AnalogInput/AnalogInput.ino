/*
  Analog Input

  Demonstrates analog input by reading an analog sensor on analog pin 0 and
  turning on and off a light emitting diode(LED) connected to digital pin 13.
  The amount of time the LED will be on and off depends on the value obtained
  by analogRead().

  The circuit:
  - potentiometer
    center pin of the potentiometer to the analog input 0
    one side pin (either one) to ground
    the other side pin to +5V
  - LED
    anode (long leg) attached to digital output 13 through 220 ohm resistor
    cathode (short leg) attached to ground

  - Note: because most Arduinos have a built-in LED attached to pin 13 on the
    board, the LED is optional.

  created by David Cuartielles
  modified 30 Aug 2011
  By Tom Igoe

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/AnalogInput
*/

int x1 = 9;   // select the input pin for the potentiometer
int x2 = 10;   // select the input pin for the potentiometer

void setup() {
  Serial.begin(115200);
}

void loop() {
  // read the value from the sensor:
  float x1_value = analogRead(x1);
  float x2_value = analogRead(x2);
  Serial.print("x1:");
  Serial.println(x1_value);
  Serial.print("x2:");
  Serial.println(x2_value);
  delay(100);
}
