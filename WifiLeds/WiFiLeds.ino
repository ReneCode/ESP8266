
#include <Adafruit_NeoPixel.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

// const char* WIFI_SSID     = "wifi-ssid";
// const char* WIFI_PASSWORD = "password";
#include "config_wifi.h"

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN            15


#define MAX_STRING_LEN   10000
char gString[MAX_STRING_LEN];

const int LOOP_DELAY_SECOND =   10;


const char LED_COL_DELIMITER = ',';
const int LED_ROWS = 4;
const int LED_COLS = 4;

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      (LED_ROWS * LED_COLS)

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_RGB + NEO_KHZ800);




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

void clearPixel() {
  int color = pixels.Color(0,0,0);
  for (int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, color);
  }
  pixels.show();
}


int randomColor() 
{
  return pixels.Color(random(255), random(255), random(255));
}



void setup() {
  Serial.begin(115200);
  Serial.println();
  
  pixels.begin(); // This initializes the NeoPixel library.
  clearPixel();
}


char *fillGlobalString(const char* sText) {
  if (strlen(sText) >= MAX_STRING_LEN) {
    return NULL;
  }
  memset(gString, 0, MAX_STRING_LEN);
  strcpy(gString, sText);
  return gString;
}

bool getTextFromJson(const char* sJson, String &sText) {
  StaticJsonBuffer<200> jsonBuffer;

  char *ptr = fillGlobalString(sJson);
  JsonObject &json = jsonBuffer.parseObject(ptr);
  if (!json.success()) {
    return false;
  }
  if (!json.containsKey("text")) {
    return false;
  }
  const char* text = json["text"];
  sText = String(text);
  return true;
}

int RGB(int r, int g, int b) {
  return pixels.Color(g,r,b);
}

void showLed(int row, int col, const char* sValue) {

  long color = 0;
  int i = 0;
  if (row%2 == 0) {
    i = row * LED_COLS + col; 
  }
  else {
    i = row * LED_COLS + (LED_COLS-1 - col);    
  }
  const int h = 60;
  const int H = 255;


  Serial.printf("idx:%d = row:%d, col:%d, val:%s\n", i, row, col, sValue);

  //  Red, Green Blue
  if ( strcmp("0", sValue) == 0)  color = 0;
  if ( strcmp("1", sValue) == 0)  color = RGB(H,H,H);
  if ( strcmp("r", sValue) == 0)  color = RGB(h,0,0);
  if ( strcmp("g", sValue) == 0)  color = RGB(0,h,0);
  if ( strcmp("b", sValue) == 0)  color = RGB(0,0,h);

  if ( strcmp("m", sValue) == 0)  color = RGB(h,0,h);
  if ( strcmp("y", sValue) == 0)  color = RGB(h,h,0);
  if ( strcmp("c", sValue) == 0)  color = RGB(0,h,h);

  pixels.setPixelColor(i, color);
  
}

void showLedRow(int row, const char *sRow) {
  int col = 0;
  const char *ptr = sRow;
  String sLed = "";
  while (ptr && *ptr != 0) {
    if (*ptr != LED_COL_DELIMITER) {
      sLed.concat(*ptr);
    }
    else {
      showLed(row, col, sLed.c_str());
      col++;
      sLed = "";
    }
    ptr++;
  }
  showLed(row, col, sLed.c_str());
}


void hideAllLeds() {
  for (int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, 0,0,0);    
  }
}

bool showLeds(String &cmd) {
  char *ptr = fillGlobalString(cmd.c_str());
  if (!ptr) {
    return false;
  }
  hideAllLeds();
  char delimiter[] = "/";
  ptr = strtok(ptr, delimiter);
  int row = 0;
  int col = 0; 
  while ( ptr) {
    showLedRow(row, ptr);
    ptr = strtok(NULL, delimiter); 
    row++;
  }
  pixels.show();
}

bool getCommandFromWifi(String &sCommand) {
  if (!connectWifi())  {
    return false;
  }
  bool bOk = false;
  HTTPClient http;
  http.begin(URL);
  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    String result = http.getString();
    bOk = getTextFromJson(result.c_str(), sCommand);
  }
  http.end();
  disconnectWifi();
  return bOk;
}


void loop() {


  String sCmd; 
  bool bOk = getCommandFromWifi(sCmd);

  delay(100);
  Serial.println("show:" + sCmd);
  delay(100);
  if (bOk) {
    showLeds(sCmd);  
  }
  else {
    clearPixel();
  }
  delay(LOOP_DELAY_SECOND * 1000);
}
