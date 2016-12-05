
const int DOOR_PIN = D7;
const int LED_PIN = D2;

uint8_t led = LOW;


void setup() {

  Serial.begin(9600);
  delay(100);
  
  pinMode(DOOR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  setLED();

  attachInterrupt(DOOR_PIN, highInterrupt, RISING);
}

void loop() {
  setLED();
}

void setLED() {
  digitalWrite(LED_PIN, led);
}

void highInterrupt() {
  led = HIGH;
  detachInterrupt(DOOR_PIN);
  attachInterrupt(DOOR_PIN, lowInterrupt, FALLING);
}


void lowInterrupt() {
  led = LOW;
  detachInterrupt(DOOR_PIN);
  attachInterrupt(DOOR_PIN, highInterrupt, RISING);

}
