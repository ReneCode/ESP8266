

#include <Arduino.h>

void wait_for_serial_connection(uint32_t baudrate)
{
  uint32_t timeout_end = millis() + 2000;
  Serial.begin(baudrate);
  while (!Serial && timeout_end > millis())
  {
  } // wait until the connection to the PC is established
}
