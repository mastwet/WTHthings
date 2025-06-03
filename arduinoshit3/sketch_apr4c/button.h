#include <Arduino.h>

#define BUTTON_COUNT 8

class Button {
  public:
    Button(byte dataPin, byte clockPin, byte latchPin);
    bool isPressed(byte buttonIndex);
  private:
    byte _dataPin, _clockPin, _latchPin;
    byte _state[BUTTON_COUNT];
    void readState();
};

Button::Button(byte dataPin, byte clockPin, byte latchPin) {
  _dataPin = dataPin;
  _clockPin = clockPin;
  _latchPin = latchPin;

  pinMode(_dataPin, INPUT);
  pinMode(_clockPin, OUTPUT);
  pinMode(_latchPin, OUTPUT);

  digitalWrite(_latchPin, LOW);
}

void Button::readState() {
  digitalWrite(_latchPin, HIGH);
  delayMicroseconds(20);
  digitalWrite(_latchPin, LOW);

  for (int i = BUTTON_COUNT - 1; i >= 0; i--) {
    digitalWrite(_clockPin, LOW);
    _state[i] = (_state[i] << 1) | digitalRead(_dataPin);
    digitalWrite(_clockPin, HIGH);
  }
}

bool Button::isPressed(byte buttonIndex) {
  readState();
  return bitRead(_state[buttonIndex / 8], buttonIndex % 8) == LOW;
}
