void setup() {
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  
  //set the resolution to 12 bits (0-4096)
  analogReadResolution(16);
}

void loop() {
  // read the analog / millivolts value for pin 2:
  int analogValue = analogRead(9);
  //int analogVolts = analogReadMilliVolts(2);
  
  // print out the values you read:
  Serial.printf("ADC0 analog value = %d\n",analogValue);
  //Serial.printf("ADC millivolts value = %d\n",analogVolts);  // read the analog / millivolts value for pin 2:
  
  analogValue = analogRead(10);
  //analogVolts = analogReadMilliVolts(2);
  
  // print out the values you read:
  Serial.printf("ADC1 analog value = %d\n",analogValue);
  //Serial.printf("ADC1 millivolts value = %d\n",analogVolts);
  
  delay(100);  // delay in between reads for clear read from serial
}
