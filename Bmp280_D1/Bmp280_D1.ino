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
 *  
 *  Speichern der Daten im Flash
 *    Adresse           Inhalt
 *      0               Anzahl der Datensätze (int)
 *      
 *      sizeof(int)     temp[0], pressure[0]  (float, float)
 *      
 *      sizeof(int) +    
 *        2*sizeof(float) temp[1], pressure[1]  (float, float)
 *      
 *      sizeof(int) +    
 *        4*sizeof(float) temp[2], pressure[2]  (float, float)
 *
 */

#include <Adafruit_BMP280.h>
#include "EEPROM.h"

Adafruit_BMP280 bmp; 

const int SLEEP_SECOND = 10;
const int BEEP_PIN = 15;

#define MAX_DATA_COUNT 100



typedef struct {
  float temperature;
  float pressure;
} data_t;

int     data_count;
data_t  data_store;

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

//  eraseDataStore();
}

void beep() {
  digitalWrite(BEEP_PIN, true);
  delay(10);
  digitalWrite(BEEP_PIN, false);
}


void eraseDataStore() {
  // Reset EEPROM bytes to '0' for the length of the data structure
  EEPROM.begin(512);
  data_count = 0;
  EEPROM.write(0, data_count);
  delay(200);
  EEPROM.commit();
  EEPROM.end();
}


void loadData(int count) {
  EEPROM.begin(512);
  int address = count * ( 2*sizeof(float));
  EEPROM.get(address, data_store);
  EEPROM.end();
}


void loadDataCount() {
  // Loads configuration from EEPROM into RAM
  EEPROM.begin(512);
  EEPROM.get(0, data_count);
  Serial.print(data_count);
  EEPROM.end();
}

void saveDataStore() {
  // Save configuration from RAM into EEPROM
  EEPROM.begin(512);
  int address = data_count * ( 2 * sizeof(float) );
  EEPROM.put(address, data_store);
  data_count = data_count+1;
  EEPROM.put(0, data_count);  
  delay(200);
  EEPROM.commit();                      // Only needed for ESP8266 to get data written
  delay(100);
}

void storeData(float temperature, float pressure) {
  //  eraseConfig();
  loadDataCount();
  data_store.temperature = temperature;
  data_store.pressure = pressure;
  saveDataStore();
}


void printData() {
  Serial.print("count:");
  Serial.print(data_count);
  Serial.println();

  for (int i=0; i<data_count; i++) {
    loadData(i);
    Serial.print("temp:");
    Serial.print(data_store.temperature);
    Serial.print("    pressure:");
    Serial.print(data_store.pressure);
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
    //storeData(22.99, 1234.56);
    
    printData();
}

void loop() {
}


