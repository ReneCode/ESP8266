
#include <RBDdimmer.h>

#define userPin 12
#define zcPin 14

const int WANTED_POWER_RESERVE = 150;
const int HYSTERESIS = 30;

dimmerLamp dimmer(userPin, zcPin);

void setup_dimmer()
{
  // Initialize dimmer (if needed)
  dimmer.setPower(0);
  dimmer.begin(NORMAL_MODE, ON_OFF_typedef::ON);
}

void update_dimmer(int current_power)
{
  int availiablePower = current_power - WANTED_POWER_RESERVE;
  int delta = 5;
  if (availiablePower > 200 || availiablePower < 0)
  {
    delta = 20;
  }

  int oldPercentPower = dimmer.getPower();
  int newPercentPower = oldPercentPower;
  if (availiablePower > 0)
  {
    newPercentPower = oldPercentPower + delta;
  }
  else
  {
    newPercentPower = oldPercentPower - delta;
  }
  newPercentPower = constrain(newPercentPower, 0, 100);

  dimmer.setPower(newPercentPower);
  Serial.print("Current power / Adjusted Percent power: ");
  Serial.print(current_power);
  Serial.print(" / ");
  Serial.println(newPercentPower);
}