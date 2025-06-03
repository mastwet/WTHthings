#include <FastLED.h>

// How many leds in your strip?
#define NUM_LEDS 8

#define DATA_PIN  11

// Define the array of leds
CRGB leds[NUM_LEDS];

void setup() { 

    FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);  // GRB ordering is typical

}

void loop() { 
    // Loop through each LED and turn it on (all LEDs will be red)
    for(int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB::Red;
    }
    FastLED.show(); // Update the LED strip
    delay(1000); // Wait for 1 second

    // Loop through each LED and turn it off
    for(int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB::Black;
    }
    FastLED.show(); // Update the LED strip
    delay(1000); // Wait for another second before repeating
}