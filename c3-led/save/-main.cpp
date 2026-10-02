#include <WiFi.h>
#include <HTTPClient.h>

const char *ssid = "K-3000";
const char *password = "YOUR_PASSWORD";

const char *url = "http://example.com/data";

void setup()
{
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
}

void loop()
{

  ESP_LOGI("BLINK", "Starting LED Blink Task on GPIO %d", 42);

  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode > 0)
    {
      String payload = http.getString();
      Serial.printf("HTTP GET code: %d\n", httpCode);
      Serial.println("Response: " + payload);
    }
    else
    {
      Serial.printf("HTTP GET failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }
  else
  {
    Serial.println("WiFi not connected!");
  }
  delay(30000); // 30 seconds
}