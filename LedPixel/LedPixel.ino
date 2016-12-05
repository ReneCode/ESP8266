#include <Adafruit_NeoPixel.h>

#define PIXEL_PIN   D2
#define NUMPIXELS   16
#define BRIGHTNESS  200


Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);


void setup() {
  pixels.begin();
}

void loop() {

  for (int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0,50,0));
    pixels.show();
    delay(500);
  }
}
