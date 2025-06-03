
#include "Arduino.h"
#include "PCF8574.h"  // https://github.com/xreef/PCF8574_library

// Instantiate Wire for generic use at 400kHz
TwoWire I2Cone = TwoWire(0);
// Instantiate Wire for generic use at 100kHz
TwoWire I2Ctwo = TwoWire(1);

// Set i2c address
PCF8574 pcf8574(&I2Cone, 0x20);
// PCF8574 pcf8574(&I2Ctwo, 0x20, 21, 22);
// PCF8574(TwoWire *pWire, uint8_t address, uint8_t interruptPin,  void (*interruptFunction)() );
// PCF8574(TwoWire *pWire, uint8_t address, uint8_t sda, uint8_t scl, uint8_t interruptPin,  void (*interruptFunction)());
// Function interrupt
void keyChangedOnPCF8574();

unsigned long timeElapsed;

void setup()
{
  Serial.begin(112560);

  I2Cone.begin(4,3); // SDA pin 16, SCL pin 17, 400kHz frequency
  delay(1000);

  // Set pinMode to OUTPUT
  for(int i=0;i<8;i++) {
    pcf8574.pinMode(i, INPUT);
  }

  Serial.print("Init pcf8574...");
  if (pcf8574.begin()){
    Serial.println("OK");
  } else {
    Serial.println("KO");
  }
	Serial.println("START");

	timeElapsed = millis();
}

bool keyChanged = false;
void loop()
{
		PCF8574::DigitalInput di = pcf8574.digitalReadAll();
		Serial.print("READ VALUE FROM PCF P1: ");
		Serial.print(di.p0);
		Serial.print(" - ");
		Serial.print(di.p1);
		Serial.print(" - ");
		Serial.print(di.p2);
		Serial.print(" - ");
		Serial.println(di.p3);
}

void keyChangedOnPCF8574(){
	// Interrupt called (No Serial no read no wire in this function, and DEBUG disabled on PCF library)
	 keyChanged = true;
}