#include <ShiftIn.h>

// Init ShiftIn instance with a single chip
ShiftIn<2> shift;

void setup() {
	Serial.begin(115200);
	// declare pins: pLoadPin, clockEnablePin, dataPin, clockPin
	shift.begin(36, 21, 35, 37);
}

void displayValues() {
	// print out all 8 buttons
	for(int i = 0; i < shift.getDataWidth(); i++)
		Serial.print( shift.state(i) ); // get state of button i
	Serial.println();
}

void loop() {
	shift.update(); // read in all values. returns true if any button has changed
	displayValues();
	delay(1);
}