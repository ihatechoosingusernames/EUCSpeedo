/**
 * First pass at a Gotway BLE client, working from the WheelLog project at https://github.com/palachzzz/WheelLogAndroid.
 * author Andrew Ellison
 */
#include <Arduino.h>
#include <BLEDevice.h>

#include "eucspeedo.h"

using namespace euc;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting Arduino BLE Client application...");
  
  EucSpeedo speedo = EucSpeedo();
}

void loop() {
  delay(1000); // Delay a second between loops.
}
