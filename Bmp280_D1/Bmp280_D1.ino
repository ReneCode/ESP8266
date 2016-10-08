/*
 * Wire.begin() wird via default mit (4,5) 4=SDA und 5=SCL aufgerufen
 * Änderungen können nur in Adafruit_BMP280.cpp gemacht werden
 * 
 * Ausserdem muss die I2C Addresse für den BMP280 auf 0x76 geändert werden (default ist 0x77)
 */

#include <Adafruit_BMP280.h>

Adafruit_BMP280 bmp; 


void setup() {
  Serial.begin(9600);
  Serial.println(F("BMP280 test"));
  
  if (!bmp.begin(0x76)) {  
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }
}

void loop() {
    Serial.print(F("Temperature = "));
    Serial.print(bmp.readTemperature());
    Serial.println(" *C");
    
    Serial.print(F("Pressure = "));
    Serial.print(bmp.readPressure());
    Serial.println(" Pa");

    Serial.print(F("Approx altitude = "));
    Serial.print(bmp.readAltitude(1013.25)); // this should be adjusted to your local forcase
    Serial.println(" m");

    Serial.println();
    delay(5 * 1000);
}
