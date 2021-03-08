/**
 * First pass at a Gotway BLE client, working from the WheelLog project at https://github.com/palachzzz/WheelLogAndroid.
 * author Andrew Ellison
 */
#include <Arduino.h>
#include <BLEDevice.h>

#include "eucspeedo.h"

using namespace euc;

EucSpeedo speedo;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting EUC Speedo app");
  
  speedo = EucSpeedo();
}

void loop() {
  delay(100);
  speedo.Process();
}
