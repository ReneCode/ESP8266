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

const int SLEEP_SECOND = 10;
const int BEEP_PIN = 15;

#define MAX_DATA_COUNT 100


typedef struct {
  int count;
  float temperature[MAX_DATA_COUNT] ;
  float pressure[MAX_DATA_COUNT];
} data_t;

data_t data_store;

void setup() {
  Serial.begin(9600);
  delay(100);

  hardwareInit();
  work();

  // parameter = micro seconds
  ESP.deepSleep(SLEEP_SECOND * 1000 * 1000);
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
  digitalWrite(BEEP_PIN, true);
  delay(10);
  digitalWrite(BEEP_PIN, false);
}


void eraseDataStore() {
  // Reset EEPROM bytes to '0' for the length of the data structure
  EEPROM.begin(512);
  for (int i = 0 ; i < sizeof(data_store) ; i++) {
    EEPROM.write(i, 0);
  }
  delay(200);
  EEPROM.commit();
  EEPROM.end();
}

void loadDataStore() {
  // Loads configuration from EEPROM into RAM
  EEPROM.begin(512);
  EEPROM.get( 0, data_store );
  EEPROM.end();
}

void saveDataStore() {
  // Save configuration from RAM into EEPROM
  EEPROM.begin(512);
  EEPROM.put( 0, data_store );
  delay(200);
  EEPROM.commit();                      // Only needed for ESP8266 to get data written
  delay(100);
}

void storeData(float temperature, float pressure) {
  //  eraseConfig();
  loadDataStore();
  int index = data_store.count;
  if (index < MAX_DATA_COUNT) {
    // enough space for new data
    data_store.temperature[index] = temperature;
    data_store.pressure[index] = pressure;
  }
  else {
    // move the data - erase the oldest
  }
  data_store.count = index+1;
  saveDataStore();
}


void printData() {
  Serial.print("count of data:");
  Serial.print(data_store.count);
  Serial.println();

  for (int i=0; i<data_store.count; i++) {
    Serial.print("temp:");
    Serial.print(data_store.temperature[i]);
    Serial.print("    pressure:");
    Serial.print(data_store.pressure[i]);
    Serial.println();
    
  }
}

void work() {
    beep();

  
    float temperature = bmp.readTemperature();
    float pressure = bmp.readPressure();
/*
    Serial.print(F("Temperature = "));
    Serial.print(temperature);
    Serial.println(" *C");
    
    Serial.print(F("Pressure = "));
    Serial.print(pressure);
    Serial.println(" Pa");
    Serial.println();
*/
    storeData(temperature, pressure);
    storeData(22.99, 1234.56);
    
    printData();
}

void loop() {
}


