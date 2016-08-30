
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
// #include <ArduinoJson.h>

// const char* WIFI_SSID     = "wifi-ssid";
// const char* WIFI_PASSWORD = "password";
#include "config_wifi.h"

const char* URL_EMONCMS = "http://emoncms.org/input/post.json";

#include <DHT.h>

const int DHT_PIN = 4;
const int DHT_TYPE = DHT22;

const int LED_BLUE = 2;

const int POWER_PIN = 5;
const int SLEEP_SECOND = 10 * 60;

#define SHOW_LIGHT

DHT dht(DHT_PIN, DHT_TYPE);


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



void sendValues(float temp, float humi)
{
  HTTPClient http;  
  char msg[200];

  // convert float to string with dtostrf()
  char sTmp[10];
  char sHumi[10];
  dtostrf(temp, 4, 2, sTmp);
  dtostrf(humi, 4, 2, sHumi);
  
  // send to emoncms.org
  sprintf(msg, "%s?json={temp:%s,humi:%s}&apikey=%s", URL_EMONCMS, sTmp, sHumi, APIKEY_EMONCMS);
  http.begin(msg);
  http.GET();
  http.writeToStream(&Serial);
  http.end();
}


void work() {
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

  if (connectWifi()) {
    sendValues(temp, humi);
  }
  disconnectWifi();
  
  Serial.print("Temp:");
  Serial.print(temp);
  Serial.print("  Huminity:");
  Serial.print(humi);
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


