#include "devicehandler.h"

#include "hwconfig.h"
#include "constants.h"
#include "logging.h"

namespace euc {

DeviceHandler::DeviceHandler() {
  pinMode(PIN_BATT, INPUT);
  pinMode(PIN_CHARGE, INPUT);
}

void DeviceHandler::Update() {
  if (delay && timeout < millis()) {
    digitalWrite(PIN_LED, (digitalRead(PIN_LED)? LOW : HIGH));
    timeout += delay;
  }
}

void DeviceHandler::FlashLed(uint32_t freq) {
  delay = 0; // Ensure LED does not flash while changing this data
  pinMode(PIN_LED, OUTPUT); // Make sure pin is not pulled high
  delay = 1000 / freq;
  timeout = millis() + delay;
}

void DeviceHandler::LedOn() {
  pinMode(PIN_LED, PULLUP); // Ensures it stays on even in sleep
}

void DeviceHandler::LedOff() {
  if (delay) {
    delay = 0;
    digitalWrite(PIN_LED, LOW);
  }

  pinMode(PIN_LED, OUTPUT_OPEN_DRAIN);  // AKA off
}

double DeviceHandler::getBatteryVoltage() {
  double v = analogRead(PIN_BATT);
  return (v / 4095.0) * 2.0 * 3.3 * (kVoltageRef / 1000.0); // Taken directly from the Lilygo example code
}

double DeviceHandler::getBatteryPercentage() {
  double v = getBatteryVoltage();
  if (v >= 3.6)
    return 100;
  else if (v > 3.3)
    return (v / 3.3) * 100;
  else
    return 0;
}

}