/**
 * First pass at a Gotway BLE client, working from the WheelLog project at https://github.com/palachzzz/WheelLogAndroid.
 * author Andrew Ellison
 */
#include <functional>

#include <Arduino.h>
#include <BLEDevice.h>

#include "constants.h"
#include "gotway.h"
#include "blehandler.h"

using namespace euc;

euc::Gotway wheel = euc::Gotway();

void setup() {
  Serial.begin(115200);
  Serial.println("Starting Arduino BLE Client application...");
  
  BleHandler ble_handler([](EucType type){printf("Found %s EUC\n", kBrandName[(size_t)type]);},
    std::bind(&euc::Gotway::ProcessInput, &wheel, std::placeholders::_1, std::placeholders::_2));
}

void loop() {
  delay(1000); // Delay a second between loops.
}
