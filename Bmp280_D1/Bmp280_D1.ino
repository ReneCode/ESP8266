/*
   Wire.begin() wird via default mit (4,5) 4=SDA und 5=SCL aufgerufen
   Änderungen können nur in Adafruit_BMP280.cpp gemacht werden

   Ausserdem muss die I2C Addresse für den BMP280 auf 0x76 geändert werden (default ist 0x77)

   deep sleep:
    loop() wird nicht durchlaufen

    stattdessen wird nach der eingestellten deepSleep(time) zeit (angabe in micro-sekunden)
    wieder setup() aufgerufen

    damit deep sleep funktioniert muss Pin: gpio16 (wake) mit rst (rest) verbunden werden.
    (aber erst nach dem das programm übertragen worden ist)

    Speichern der Daten im Flash
      Adresse             Inhalt
        0                 Anzahl der Datensätze (int)

        sizeof(int)       temp[0], pressure[0]  (float, float)

        sizeof(int) +
        2*sizeof(float)   temp[1], pressure[1]  (float, float)

        sizeof(int) +
        4*sizeof(float)   temp[2], pressure[2]  (float, float)

*/

#include <Adafruit_BMP280.h>
#include <EEPROM.h>

#include "Wifi.h"
#include "config_emoncms.h"

Adafruit_BMP280 bmp;

const int SLEEP_SECOND = 10 * 60;
const int BEEP_PIN = 15;


const int EMONCMS_NODE_ID = 1;
const char* PROVIDER_URL = "http://emoncms.org/input/bulk.json";

const int FLASH_SIZE = 4096;
#define UPLOAD_BUNDLE_SIZE  20

typedef struct {
  float temperature;
  float pressure;
} data_t;



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
  // set 0 as count of data
  int count = 0x0;
  EEPROM.put(0, count);
  delay(200);
  EEPROM.commit();
}


bool loadData(int index, data_t &rData) {
  int address = sizeof(int) + index * ( 2 * sizeof(float));
  EEPROM.get(address, rData);
  return !isnan(rData.temperature)  &&
         !isnan(rData.pressure);
}


int loadDataCount() {
  int count = 0;
  EEPROM.get(0, count);
  return count;
}

void appendDataStore(int currentCount, const data_t &rData) {
  int newIndex = currentCount;
  int address = sizeof(int) + newIndex * ( 2 * sizeof(float) );
  // save the data
  EEPROM.put(address, rData);
  // save the count
  EEPROM.put(0, newIndex + 1);
  delay(200);
  EEPROM.commit();                      // Only needed for ESP8266 to get data written
  delay(100);
}

void storeData(const data_t &data) {
  int count = loadDataCount();
  appendDataStore(count, data);
}


void printData() {
  Serial.print("count:");
  int count = loadDataCount();
  Serial.print(count);
  Serial.println();
  data_t data;
  for (int i = 0; i < count; i++) {
    if (loadData(i, data)) {
      Serial.print(i);
      Serial.print(" temp:");
      Serial.print(data.temperature);
      Serial.print("    pressure:");
      Serial.print(data.pressure);
      Serial.println();    
    }
  }
}

  /*
      https://emoncms.org/input/bulk.json?data=[[520,16,1137],[530,17,1437,3164],[535,19,1412,3077]]&sentat=543
  */
void fillDataParameter(const data_t &data, String &str, int timeDelta = 0) {
  str = String("[") + String(timeDelta) + String(",") + String(EMONCMS_NODE_ID) + String(",") +
        String(data.temperature) + String(",") +
        String(data.pressure) + String("]");
}


bool uploadOneData(const data_t &data) {
  String url = PROVIDER_URL + String("?data=[");
  String strData;
  fillDataParameter(data, strData);
  url = url + strData + String("]&apikey=") + String(EMONCMS_APIKEY);

//  Serial.print("uploadOneData:");
//  Serial.println(url);

  return WifiGet(url);
}

bool uploadBundleData(int idxStart, int idxEnd) {
  String url = PROVIDER_URL + String("?data=[");
  bool bOkData = false;
  for (int i=idxStart; i<=idxEnd; i++) {
    String strData;
    data_t data;
    if (loadData(i, data)) {
      fillDataParameter(data, strData, i*SLEEP_SECOND);
      if (bOkData) {
        url += String(",");
      }
      url = url + strData;      
      bOkData = true;
    }
  }
  if (!bOkData) {
    return false;
  }

  url = url + String("]&apikey=") + String(EMONCMS_APIKEY);

  Serial.print("uploadBundleData:");
  Serial.print(idxStart);
  Serial.print(" - ");
  Serial.print(idxEnd);
  
  Serial.println(url);

  return WifiGet(url);  
}


bool uploadStoredData() {
  bool bOk = true;
  int count = loadDataCount();

  Serial.print("uploadStoredData:");
  Serial.println(count);

  
  int nBundle = count / UPLOAD_BUNDLE_SIZE;
  int nRestBundle = count % UPLOAD_BUNDLE_SIZE;
  for (int i=0; i<nBundle; i++) {
    int idxStart = i * UPLOAD_BUNDLE_SIZE;
    int idxEnd = idxStart + UPLOAD_BUNDLE_SIZE -1;
    uploadBundleData(idxStart, idxEnd);
  }
  if (nRestBundle > 0) {
    int idxStart = nBundle * UPLOAD_BUNDLE_SIZE;
    int idxEnd = count -1;
    uploadBundleData(idxStart, idxEnd);
  }

  return bOk;
}

void work() {
  EEPROM.begin(FLASH_SIZE);

  beep();

//  printData();

  data_t data;

  data.temperature = bmp.readTemperature();
  data.pressure = bmp.readPressure();

  if (WifiConnect()) {
    Serial.println("Wifi connected");
    int count = loadDataCount();
    if (count == 0) {
      Serial.println("upload one data");
      if (uploadOneData(data) == false) {
        storeData(data);
      }
    } else {
      Serial.println("store data");
      storeData(data);
      if (uploadStoredData() == true) { 
        Serial.println("erase data");      
        eraseDataStore();
      }
    }
  } else {
    Serial.println("store data");
    storeData(data);
  }

  EEPROM.end();


  delay(100);
  WifiDisconnect();
  delay(100);
}


void setup() {
  Serial.begin(9600);
  delay(100);

  hardwareInit();
  work();

  // parameter = micro seconds
  ESP.deepSleep(SLEEP_SECOND * 1000 * 1000);
  delay(100);
}


void loop() {
}


