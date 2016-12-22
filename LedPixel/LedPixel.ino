#include <Adafruit_NeoPixel.h>

#define PIXEL_PIN   D2
#define NUMPIXELS   16
#define BRIGHTNESS  100


Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);


void setup() {
  pixels.begin();
  randomSeed(analogRead(0));
//  pixels.setBrightness(0);
}


void setRandomPixels() {
  for (int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, random(256), random(256), random(256) );
  }
}

void setAllPixels(int color) {
  for (int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, color);
    pixels.show();
    delay(50);
  }
}


uint8_t getColor(int w) {
  return pixels.Color(10,200,140,w);
}

void loop() {

  for (int i=0; i<100; i+=20) {
    setAllPixels( getColor(i) );
    pixels.show();
    delay(20);
  }

  for (int i=100; i>0; i-=20) {
    setAllPixels( pixels.Color(200,100,50, random(256)) );
    pixels.show();
    delay(20); 
  }

}

