/*
 * Wire.begin() wird via default mit (4,5) 4=SDA und 5=SCL aufgerufen
 * Änderungen können nur in Adafruit_BMP280.cpp gemacht werden
 * 
 * Ausserdem muss die I2C Addresse für den BMP280 auf 0x76 geändert werden (default ist 0x77)
 * 
 * deep sleep:
 *  loop() wird nicht durchlaufen
 *  
 *  stattdessen wird nach der eingestellten deepSleep(time) zeit (angabe in micro-sekunden)
 *  wieder setup() aufgerufen
 *  
 *  damit deep sleep funktioniert muss Pin: gpio16 (wake) mit rst (rest) verbunden werden.
 *  (aber erst nach dem das programm übertragen worden ist)
 */

#include <Adafruit_BMP280.h>
#include "EEPROM.h"

Adafruit_BMP280 bmp; 


const int SLEEP_US = 5 * 1000 * 1000;
const int BEEP_PIN = 15;

const int FLASH_ADR = 0;
#define BUFFER_LEN  20


typedef struct {
  int value;
} config_t;

config_t cfg;

void setup() {
  Serial.begin(9600);
  Serial.println(F("BMP280 test"));

  hardwareInit();
  work();

  Serial.println();

  ESP.deepSleep(SLEEP_US);
  delay(100);
}



void hardwareInit() {
  pinMode(BEEP_PIN, OUTPUT);
  if (!bmp.begin(0x76)) {  
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }
}

void beep() {
//  int nr = readFlash();

  loadConfig();
  cfg.value++;
  Serial.print("nr:");
  Serial.print(cfg.value);
  Serial.println();
    
  digitalWrite(BEEP_PIN, true);
  delay(10);
  digitalWrite(BEEP_PIN, false);

  saveConfig();
//  eraseConfig();
}


void eraseConfig() {
  // Reset EEPROM bytes to '0' for the length of the data structure
  EEPROM.begin(512);
  for (int i = 0 ; i < sizeof(cfg) ; i++) {
    EEPROM.write(i, 0);
  }
  delay(200);
  EEPROM.commit();
  EEPROM.end();
}

void loadConfig() {
  // Loads configuration from EEPROM into RAM
  EEPROM.begin(512);
  EEPROM.get( 0, cfg );
  EEPROM.end();
}

void saveConfig() {
  // Save configuration from RAM into EEPROM
  EEPROM.begin(512);
  EEPROM.put( 0, cfg );
  delay(200);
  EEPROM.commit();                      // Only needed for ESP8266 to get data written
}


void work() {
    beep();
    /*
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
    */
}

void loop() {
}


