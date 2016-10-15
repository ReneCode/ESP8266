
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>


// const char* WIFI_SSID     = "wifi-ssid";
// const char* WIFI_PASSWORD = "password";
#include "config_wifi.h"


bool WifiConnect() 
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int maxTry = 5;
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

void WifiDisconnect() 
{
  WiFi.disconnect();
}


int WifiGet(const String &url)
{
  HTTPClient http;
/*
Serial.print("Wifi Get:");
Serial.print(url);
Serial.println();
*/
  http.begin(url);
  int ret = http.GET();
  http.writeToStream(&Serial);
  http.end();
  return ret;
}



