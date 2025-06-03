/*
 * Controlling and looping through a CD74HC4067's channel outputs
 *
 *
 * Connect the four control pins to any unused digital or analog pins.
 * This example uses digital pins 4, 5, 6, and 7.
 *
 * Connect the common pin to any other available pin. This is the pin that will be
 * shared between the 16 channels of the CD74HC4067. The 16 channels will inherit the
 * capabilities of the common pin. For example, if it's connected to an analog pin,
 * you will be able to use analogRead on each of the 16 channels.
 *
*/

#include <CD74HC4067.h>
#include <ADCInput.h>

ADCInput adc0(A0);
               // s0 s1 s2 s3
CD74HC4067 my_mux(18, 19, 20, 21);  // create a new CD74HC4067 object with its four control pins

void setup()
{
    Serial.begin(9600);
    adc0.begin(8000);
}

void loop()
{
    for (int i = 0; i < 16; i++) {
        my_mux.channel(i);

        Serial.printf("channel%d:%d\n",i,adc0.read());
    }
    delay(1000);
}
