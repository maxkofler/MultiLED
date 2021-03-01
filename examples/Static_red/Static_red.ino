#include <MultiLED.h>

//The ammount of leds on the strip
#define ledC 30

//Define a RGB array for the LEDs
RGB leds[ledC];

//Create a pointer to our object
MultiLED *multiLed;

void setup() 
{
  //Initializing PORT B
  DDRB = 0xFF;

  //Initialize the library
  multiLed = new MultiLED(MULTILED_PORTB);
  
  //Commit the library how many strips there will be and what's the max count of leds on a strip
  multiLed->addStrips(1, ledC);

  //Add the RGB array to the library
  multiLed->addRGBArray(0, ledC, leds);

  //Set all leds to red (1, 0, 0) format: R, G, B
  for (int i = 0; i < ledC; i++){
    leds[i] = {10, 0, 0};
  }

  //Now write the values to the strip
  multiLed->show();
}

void loop() {
}
