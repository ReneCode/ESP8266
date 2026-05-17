
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <utility.h>

#include "secrets.h"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

char iotServerUrl[] = IOT_SERVER_URL;
char iotToken[] = IOT_TOKEN;

const int LED_DELAY = 200;

// on esp d1_mini, D6 is GPIO12
#define LED_RED_PIN D6   // D6 on D1 Mini
#define LED_GREEN_PIN D5 // D5 on D1 Mini

void connectToWiFi()
{
  Serial.println();
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30)
  {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println();
    Serial.println("WiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println();
    Serial.println("WiFi connection failed!");
  }
}

int makeHttpRequest()
{
  int result = 0;

  if (WiFi.status() == WL_CONNECTED)
  {
    WiFiClient client;
    HTTPClient http;

    const char *powerSensor = "sensor.powermeter_power";
    const String serverUrl = String(iotServerUrl) + "/api/states/" + powerSensor;

    Serial.println();
    Serial.print("Making HTTP request to: ");
    Serial.println(iotServerUrl);

    http.begin(client, serverUrl);
    http.addHeader("Authorization", String("Bearer ") + iotToken);

    int httpCode = http.GET();

    if (httpCode > 0)
    {
      Serial.print("HTTP Response code: ");
      Serial.println(httpCode);

      if (httpCode == HTTP_CODE_OK)
      {
        String payload = http.getString();
        // Serial.println("Response:");
        // Serial.println(payload);

        // Parse JSON response
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, payload);

        if (error)
        {
          Serial.print("JSON parsing failed: ");
          Serial.println(error.c_str());
        }
        else
        {
          // Extract entity_id and state
          String entityId = doc["entity_id"].as<String>();
          int state = doc["state"].as<int>();

          Serial.println("\nParsed values:");
          Serial.print("Entity ID: ");
          Serial.println(entityId);
          Serial.print("State: ");
          Serial.println(state);

          result = state;
        }
      }
    }
    else
    {
      Serial.print("HTTP request failed, error: ");
      Serial.println(http.errorToString(httpCode));
    }

    http.end();
  }
  else
  {
    Serial.println("WiFi not connected!");
  }
  return result;
}

void setup()
{

  wait_for_serial_connection(115200);

  pinMode(LED_RED_PIN, OUTPUT);
  digitalWrite(LED_RED_PIN, LOW);

  pinMode(LED_GREEN_PIN, OUTPUT);
  digitalWrite(LED_RED_PIN, LOW);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // Initialize Serial for debugging

  Serial.println("\n\nStarting ESP8266...");

  // Connect to WiFi
  connectToWiFi();

  // Make initial HTTP request
  if (WiFi.status() == WL_CONNECTED)
  {
    makeHttpRequest();
  }
}

#define POWER_RESERVE 10

void loop()
{

  static int power = 0;

  const int requestInterval = 3000;
  // Make HTTP request every 3 seconds
  static unsigned long lastRequest = 0;
  if (millis() - lastRequest > requestInterval)
  {
    power = makeHttpRequest();

    lastRequest = millis();
  }

  // if power is negative then the PV produces more than the house consumes,
  // :-)
  if (power < -POWER_RESERVE)
  {
    // blink green LED to show activity
    digitalWrite(LED_GREEN_PIN, HIGH);
    delay(50);
    digitalWrite(LED_GREEN_PIN, LOW);
    delay(3000);
  }
  else
  {
    // Blink LED to show activity
    digitalWrite(LED_RED_PIN, HIGH);
    delay(50);
    digitalWrite(LED_RED_PIN, LOW);
    delay(1000);
  }
}
