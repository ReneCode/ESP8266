

#include <FastLED.h>
#include <stdint.h>
#include "rand_int.h"

#define LED_PIN     4  //  esp2866 Pin D2
#define NUM_LEDS    16
#define BRIGHTNESS  100
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define DELAY       100

void wait_for_serial_connection() {
  uint32_t timeout_end = millis() + 2000;
  Serial.begin(115200);
  while(!Serial && timeout_end > millis()) {}  //wait until the connection to the PC is established
  
}

void setup() {
  wait_for_serial_connection(); // Optional, but seems to help Teensy out a lot.
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() {
    // for (int i=0; i<NUM_LEDS; i++) {


    int i = rand_int(NUM_LEDS);
    CRGB color = CRGB(rand_int(255), rand_int(255), rand_int(255));
    leds[i] = color;
//   leds[i] = CRGB::Red;
  FastLED.show();
  delay(DELAY);
  leds[i] = CRGB::Black;
  FastLED.show();
//   delay(DELAY);
    // }
}