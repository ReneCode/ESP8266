
#include <DHT.h>

const int DHT_PIN = 4;
const int DHT_TYPE = DHT22;

const int LED_BLUE = 2

const int POWER_PIN = 5;
const int SLEEP_SECOND = 10;

#define SHOW_LIGHT

DHT dht(DHT_PIN, DHT_TYPE);


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
  
  Serial.print("Temp:");
  Serial.print(temp);
  Serial.print("  Huminity:");
  Serial.print(humi);
  Serial.println();
  

}

void loop() {
}
