#include <Adafruit_NeoPixel.h>

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN            15

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      16


Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_RGB + NEO_KHZ800);



void setup() {
  pixels.begin(); // This initializes the NeoPixel library.
}

void clearPixel(int order[]) {
  int color = pixels.Color(0,0,0);
  for (int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, color);
  }
  pixels.show();
}


int randomColor() 
{
  return pixels.Color(random(255), random(255), random(255));
}



void loop() {

  for (int i=0; i<NUMPIXELS; i++) 
  {
    int color = randomColor();
    pixels.setPixelColor(i, color);
  }
  pixels.show();
  delay( 1000  );
}
