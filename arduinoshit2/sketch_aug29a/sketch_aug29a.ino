const int controlPins[4] = {18, 19, 20,21};  // Pins connected to S0, S1, S2, and S3 of CD74HC4067
const int analogPin = 26;                  // Analog pin connected to SIG pin of CD74HC4067

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 4; i++) {
    pinMode(controlPins[i], OUTPUT);
  }
}

void loop() {
  for (int channel = 0; channel < 16; channel++) {
    // Select the desired channel on the multiplexer
    selectChannel(channel);
    
    // Read analog value from the selected channel
    int adcValue = analogRead(analogPin);
    
    Serial.print("Channel ");
    Serial.print(channel);
    Serial.print(": ADC Value = ");
    Serial.println(adcValue);
    
    delay(1000); // Delay before reading the next channel
  }
}

void selectChannel(int channel) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(controlPins[i], (channel >> i) & 1);
  }
}