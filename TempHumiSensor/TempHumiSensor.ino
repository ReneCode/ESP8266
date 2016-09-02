
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>


// const char* WIFI_SSID     = "wifi-ssid";
// const char* WIFI_PASSWORD = "password";
#include "config_wifi.h"

const char* URL_EMONCMS = "http://emoncms.org/input/post.json";

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>


#include <DHT.h>

const int SDA_PIN = 12;
const int SCL_PIN = 14;

const int DHT_PIN = 4;
const int DHT_TYPE = DHT22;

const int LED_BLUE = 2;

const int POWER_PIN = 5;
const int SLEEP_SECOND = 10 * 60;

#define SHOW_LIGHT

DHT dht(DHT_PIN, DHT_TYPE);

Adafruit_BMP085_Unified bmp;

ADC_MODE(ADC_VCC); // to use getVcc

bool connectWifi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int maxTry = 10;
  bool connected = false;
  while (!connected  &&  maxTry > 0)
  {
    delay(1000);
    connected = WiFi.status() == WL_CONNECTED;
    maxTry--;
  }
  if (connected)
  {
    Serial.println("Wifi connected");
  }
  else
  {
    Serial.println("Wifi not connected");
  }
  return connected;
}

void disconnectWifi() {
  WiFi.disconnect();
}



void sendValues(float vcc, float temp, float humi, float bmpTemp, float pressure)
{
  HTTPClient http;
  char msg[200];

  // convert float to string with dtostrf()
  char sVcc[10];
  char sTmp[10];
  char sHumi[10];
  char sBmpTmp[10];
  char sPressure[10];
  dtostrf(vcc, 4, 2, sVcc);
  dtostrf(temp, 4, 2, sTmp);
  dtostrf(humi, 4, 2, sHumi);
  dtostrf(bmpTemp, 4, 2, sBmpTmp);
  dtostrf(pressure, 6, 2, sPressure);

  // send to emoncms.org
  sprintf(msg, "%s?json={vcc:%s,temp:%s,humi:%s,bmpTemp:%s,pressure:%s}&apikey=%s",
          URL_EMONCMS, sVcc, sTmp, sHumi, sBmpTmp, sPressure, APIKEY_EMONCMS);
  Serial.println(msg);
  http.begin(msg);
  http.GET();
//  http.writeToStream(&Serial);
  http.end();
}


bool getBmpData(float *pTmp, float *pPressure)
{
  sensors_event_t event;
  bmp.getEvent(&event);
  if (!event.pressure) {
    return false;
  }

  *pPressure = event.pressure;
  bmp.getTemperature(pTmp);
  return true;
}

void work() {
  Wire.begin(SDA_PIN, SCL_PIN);
  if (!bmp.begin()) {
    Serial.println("error init pressure sensor");
  }

  Serial.println("start measuring\n");
  digitalWrite(POWER_PIN, HIGH);

  delay(100);
  dht.begin();
  // wait > 2 second - sensor has measured the data
  delay(3 * 1000);

  float humi = dht.readHumidity();
  float temp = dht.readTemperature();

  digitalWrite(POWER_PIN, LOW);

  if (isnan(humi) ||  isnan(temp)) {
    Serial.println("Failed to read from DHT sensor");
    return;
  }

  float bmpTemp = 0.0;
  float bmpPressure = 0.0;
  getBmpData(&bmpTemp, &bmpPressure);

  float vcc = (float)ESP.getVcc() / 1000.0;

  if (connectWifi()) {
    sendValues(vcc, temp, humi, bmpTemp, bmpPressure);
  }
  disconnectWifi();


  Serial.println();
}

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("start");


  pinMode(POWER_PIN, OUTPUT);

#ifdef SHOW_LIGHT
  pinMode(LED_BLUE, OUTPUT);
  delay(10);
  digitalWrite(LED_BLUE, LOW);
  delay(10);
  digitalWrite(LED_BLUE, HIGH);
#endif

  work();

  ESP.deepSleep(SLEEP_SECOND * 1E6);
  delay(100);
}

void loop() {
}


