#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

/*

BMP180
barometric pressure sensor
datasheet: https://cdn-shop.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf

library: https://github.com/adafruit/Adafruit_BMP085_Unified
and: https://github.com/adafruit/Adafruit_Sensor

  Top view
 ---------------
|               |
|               |
|               |
 VIN GND SCL SDA
 ---------------

*/

const int SDA_PIN = 12;
const int SCL_PIN = 14;


Adafruit_BMP085_Unified bmp;


void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("start");
  Wire.begin(SDA_PIN, SCL_PIN);
  if (bmp.begin()) {
    Serial.println("bmp read");
  }
  Serial.println("setup finished");
}

void loop() {

  sensors_event_t event;
  bmp.getEvent(&event);
  if (event.pressure) {

    Serial.print(" pressure (hPa):");
    Serial.print(event.pressure);
    
    float bmpTemp = 0.0;
    bmp.getTemperature(&bmpTemp);
    float bmpPressure = 0.0;
    bmp.getPressure(&bmpPressure);

    Serial.print(" Temp2:");
    Serial.print(bmpTemp);
    Serial.print("  Pressure:");
    Serial.print(bmpPressure);
  }
  
  Serial.println();
  delay(2000);
}


